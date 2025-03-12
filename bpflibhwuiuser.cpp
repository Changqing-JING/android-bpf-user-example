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

int attachBPF(const char *tp_prog_path, const char *probeName,
              uint32_t symbolOffset) {

  // Attach tracepoint and wait for 4 seconds
  int mProgFd = bpf_obj_get(tp_prog_path);
  if (mProgFd < 0) {
    std::cout << "bpf_obj_get(tp_prog_path) failed " << tp_prog_path
              << std::endl;
    return 1;
  }
  bpf_detach_uprobe(probeName);
  const char *soPath = "/system/lib64/libhwui.so";

  int ret = bpf_attach_uprobe(mProgFd, BPF_PROBE_ENTRY, probeName, soPath,
                              symbolOffset, -1, 0);

  return ret;
}

int main() {

  const char *tp_map_path = "/sys/fs/bpf/map_bpflibhwui_drawtextstring_map";

  int mMapFd = bpf_obj_get(tp_map_path);
  if (mMapFd < 0) {
    std::cout << "bpf_obj_get(tp_map_path) failed" << std::endl;
    return 1;
  }

  int32_t const trace_fd = open("/sys/kernel/tracing/trace_pipe", O_RDONLY, 0);
  if (trace_fd < 0) {
    std::cout << "open trace_pipe failed" << std::endl;
    return 1;
  }

  int ret = attachBPF("/sys/fs/bpf/prog_bpflibhwui_uprobe_drawtextstring",
                      "uprobe/drawTextString", 0x0000000000203580);

  if (ret < 0) {
    std::cout << "open drawTextString failed" << std::endl;
    return 1;
  }

  ret = attachBPF("/sys/fs/bpf/prog_bpflibhwui_uprobe_translate",
                  "uprobe/translate", 0x0000000000201e60);

  if (ret < 0) {
    std::cout << "open drawTextString failed" << std::endl;
    return 1;
  }

  std::thread readTrace([trace_fd]() {
    while (true) {
      char buf[4096];
      ssize_t const sz = read(trace_fd, buf, sizeof(buf) - 1);
      if (sz > 0) {
        buf[sz] = 0;
        std::cout << buf << std::endl;
      }
    }
  });

  readTrace.join();
  return 0;
}