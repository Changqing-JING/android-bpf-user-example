#include <bpf/BpfMap.h>
#include <cstdlib>
#include <libbpf.h>
#include <log/log.h>
#include <thread>

int main() {
  const char *tp_prog_path =
      "/sys/fs/bpf/prog_bpftest_tracepoint_sched_sched_switch";
  const char *tp_map_path = "/sys/fs/bpf/map_bpftest_cpu_pid_map";

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

  int ret = bpf_attach_tracepoint(mProgFd, "sched", "sched_switch");

  if (ret < 0) {
    printf("bpf_attach_tracepoint failed\n");
    return 1;
  }

  // Read the map to find the last PID that ran on CPU 0
  android::bpf::BpfMap<int, int> myMap(tp_map_path);

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto value = myMap.readValue(0);
    if (value.has_value()) {
      printf("last PID running on CPU %d is %d\n", 0, value.value());
    } else {
      printf("read map failed\n");
    }
  }

  return 0;
}