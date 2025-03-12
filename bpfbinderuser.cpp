#include <bpf/BpfMap.h>
#include <cstdlib>
#include <iostream>
#include <libbpf.h>
#include <log/log.h>
#include <thread>

int main() {
  const char *tp_prog_path =
      "/sys/fs/bpf/prog_bpfbinder_tracepoint_binder_binder_ioctl";
  const char *tp_map_path = "/sys/fs/bpf/map_bpfbinder_binder_ioctl_map";

  // Attach tracepoint and wait for 4 seconds
  int mProgFd = bpf_obj_get(tp_prog_path);
  if (mProgFd < 0) {
    printf("bpf_obj_get(%s) failed\n", tp_prog_path);
    perror("reason:");
    return 1;
  }
  int mMapFd = bpf_obj_get(tp_map_path);
  if (mMapFd < 0) {
    printf("bpf_obj_get(%s) failed\n", tp_map_path);
    perror("reason:");
    return 1;
  }

  bpf_detach_tracepoint("binder", "binder_ioctl");

  int ret = bpf_attach_tracepoint(mProgFd, "binder", "binder_ioctl");

  if (ret < 0) {
    printf("bpf_attach_tracepoint failed\n");
    return 1;
  } else {
    printf("bpf_attach_tracepoint success\n");
  }

  // Read the map to find the last PID that ran on CPU 0
  android::bpf::BpfMap<uint32_t, uint8_t> myMap(tp_map_path);

  int32_t const trace_fd = open("/sys/kernel/tracing/trace_pipe", O_RDONLY, 0);
  if (trace_fd < 0) {
    std::cout << "open trace_pipe failed" << std::endl;
  }

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

  readTrace.join();

  return 0;
}