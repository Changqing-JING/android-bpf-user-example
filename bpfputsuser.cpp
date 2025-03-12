#include "BpfSyscallWrappers.h"
#include <bpf/BpfMap.h>
#include <cstdlib>
#include <cstring>
#include <elf.h>
#include <fstream>
#include <iostream>
#include <libbpf.h>
#include <log/log.h>
#include <stdexcept>
#include <thread>
#include <vector>

std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cout << "Cannot open file: " << filename << '\n';
    std::terminate();
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) {
    std::cout << "Cannot read file: " << filename << '\n';
    std::terminate();
  }

  return buffer;
}

uint32_t parseELF(const std::vector<char> &buffer,
                  const std::string &symbol_name) {
  const Elf64_Ehdr *ehdr = reinterpret_cast<const Elf64_Ehdr *>(buffer.data());

  if (std::memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
    std::cout << "Not an ELF file\n";
    std::terminate();
  }

  const Elf64_Shdr *shdrs =
      reinterpret_cast<const Elf64_Shdr *>(buffer.data() + ehdr->e_shoff);
  const char *shstrtab = buffer.data() + shdrs[ehdr->e_shstrndx].sh_offset;

  const Elf64_Shdr *symtab = nullptr;
  const Elf64_Shdr *strtab = nullptr;

  for (int i = 0; i < ehdr->e_shnum; ++i) {
    if (shdrs[i].sh_type == SHT_SYMTAB) {
      symtab = &shdrs[i];
    } else if (shdrs[i].sh_type == SHT_STRTAB &&
               std::strcmp(shstrtab + shdrs[i].sh_name, ".strtab") == 0) {
      strtab = &shdrs[i];
    }
  }

  if (!symtab || !strtab) {
    std::cout << "Symbol table or string table not found\n";
    std::terminate();
  }

  const Elf64_Sym *symbols =
      reinterpret_cast<const Elf64_Sym *>(buffer.data() + symtab->sh_offset);
  const char *strtab_data = buffer.data() + strtab->sh_offset;

  for (unsigned int i = 0; i < symtab->sh_size / symtab->sh_entsize; ++i) {
    if (std::strcmp(strtab_data + symbols[i].st_name, symbol_name.c_str()) ==
        0) {
      return static_cast<uint32_t>(symbols[i].st_value);
    }
  }

  std::cout << "Symbol " << symbol_name << " not found\n";
  std::terminate();
}

union Data {
  int value;
  char msg[4];
};

int main() {
  const char *tp_prog_path = "/sys/fs/bpf/prog_bpfputs_uprobe_puts";
  const char *tp_map_path = "/sys/fs/bpf/map_bpfputs_puts_map";

  // Attach tracepoint and wait for 4 seconds
  int mProgFd = android::bpf::bpfFdGet(tp_prog_path, BPF_F_RDONLY);
  if (mProgFd < 0) {
    std::cout << "bpf_obj_get " << tp_prog_path << " failed" << std::endl;
    perror("reason: ");
    return 1;
  }
  int mMapFd = bpf_obj_get(tp_map_path);
  if (mMapFd < 0) {
    std::cout << "bpf_obj_get " << tp_map_path << " failed" << std::endl;
    perror("reason: ");
    return 1;
  }

  int32_t const trace_fd = open("/sys/kernel/tracing/trace_pipe", O_RDONLY, 0);
  if (trace_fd < 0) {
    std::cout << "open trace_pipe failed" << std::endl;
  }

  bpf_detach_uprobe("uprobe/puts");
  const char *soPath = "/apex/com.android.runtime/lib64/bionic/libc.so";
  std::vector<char> buffer = readFile(soPath);
  uint32_t symbolOffset = parseELF(buffer, "puts");

  int ret = bpf_attach_uprobe(mProgFd, BPF_PROBE_ENTRY, "uprobe/puts", soPath,
                              symbolOffset, -1, 0);

  if (ret < 0) {
    std::cout << "bpf_attach_tracepoint failed " << ret << std::endl;
    perror("reason: ");
    return 1;
  }

  // Read the map to find the last PID that ran on CPU 0
  android::bpf::BpfMap<int, int> myMap(tp_map_path);

  std::thread readTrace([trace_fd]() {
    char buf[4096];
    if (trace_fd >= 0) {
      while (true) {
        ssize_t const sz = read(trace_fd, buf, sizeof(buf) - 1);
        if (sz > 0) {
          buf[sz] = 0;
          std::cout << buf << std::endl;
        }
      }
    }
  });

  while (true) {

    auto optValue = myMap.readValue(0);
    if (optValue.has_value()) {
      Data data;
      data.value = optValue.value();

      if (data.value != 0) {
        std::cout << "libc:puts called with param: " << data.msg << std::endl;
        myMap.writeValue(0, 0, BPF_ANY);
      }
    } else {
      std::cout << "read map failed" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  readTrace.join();
  return 0;
}