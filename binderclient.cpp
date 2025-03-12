#include <binder/IBinder.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <com/example/IMyInterface.h>
#include <iostream>
#include <utils/String16.h>

using namespace android;
using namespace com::example;

int main() {
  android::ProcessState::initWithDriver("/dev/vndbinder");
  // Get the default service manager
  sp<IServiceManager> sm = defaultServiceManager();

  // Retrieve the service by name
  sp<IBinder> binder = sm->getService(String16("com.example.IMyInterface"));
  if (binder == nullptr) {
    std::cerr << "Failed to get the service!" << std::endl;
    return -1;
  }

  // Cast the binder to the IMyInterface
  sp<IMyInterface> service = interface_cast<IMyInterface>(binder);
  if (service == nullptr) {
    std::cerr << "Failed to cast the binder to IMyInterface!" << std::endl;
    return -1;
  }

  // Call the remote method
  std::vector<uint8_t> message;
  auto status = service->getMessage(&message);
  if (status.isOk()) {
    std::cout << "Message from server: " << message.data() << std::endl;
  } else {
    std::cerr << "Failed to call getMessage!" << std::endl;
  }

  return 0;
}
