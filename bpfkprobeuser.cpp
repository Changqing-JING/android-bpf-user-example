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

union Data {
  int value;
  char msg[4];
};

int main() {
  const char *tp_prog_path =
      "/sys/fs/bpf/prog_bpfkprobe_kprobe___seccomp_filter";
  const char *tp_map_path =
      "/sys/fs/bpf/map_bpfkprobe_kprobe_seccomp_filter_map";

  // Attach tracepoint and wait for 4 seconds
  int mProgFd = bpf_obj_get(tp_prog_path);
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

  bpf_detach_kprobe("__seccomp_filter");

  int ret = bpf_attach_kprobe(mProgFd, BPF_PROBE_ENTRY, "kprobe",
                              "__seccomp_filter", 0, 0);

  if (ret < 0) {
    std::cout << "bpf_attach_kprobe failed " << ret << std::endl;
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