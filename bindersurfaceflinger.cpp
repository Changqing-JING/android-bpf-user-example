#include <binder/IBinder.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <iostream>
#include <unistd.h> // For read() and close()
#include <utils/String16.h>

using namespace android;

int main() {
  // Get the default service manager
  sp<IServiceManager> sm = defaultServiceManager();

  // Retrieve the service by name
  sp<IBinder> binder = sm->getService(String16("SurfaceFlinger"));
  if (binder == nullptr) {
    std::cerr << "Failed to get the service!" << std::endl;
    return -1;
  }

  int sfd[2];
  int error = pipe(sfd);

  if (error != 0) {
    std::cerr << "Failed to create pipe: " << strerror(errno) << std::endl;
    return -1;
  }

  Vector<String16> args{};

  status_t err = binder->dump(sfd[1], args);

  if (err != OK) {
    std::cerr << "Failed to dump service: " << strerror(err) << std::endl;
    return -1;
  }

  // Close write end of pipe immediately after dumping
  // This ensures read() won't block forever when all data is read
  close(sfd[1]);

  // Read from pipe
  char buffer[4096];
  ssize_t bytes_read;

  while ((bytes_read = read(sfd[0], buffer, sizeof(buffer) - 1)) > 0) {
    buffer[bytes_read] = '\0';
    std::cout << buffer;
  }

  // Close read end of pipe
  close(sfd[0]);

  return 0;
}
