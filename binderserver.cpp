#include "com/example/BnMyInterface.h"
#include <atomic>
#include <binder/Binder.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <com/example/IMyInterface.h>

using namespace android;
using namespace com::example;

std::atomic<int> counter;

class MyService : public BnMyInterface {
public:
  binder::Status getMessage(std::vector<uint8_t> *_aidl_return) override {
    // Provide the implementation for getMessage
    std::string counterStr = std::to_string(counter++);
    std::vector<uint8_t> msg;

    // Append "Hello from the server" and the counter to the message
    std::string baseMessage = "Hello from the server";
    msg.insert(msg.end(), baseMessage.begin(), baseMessage.end());
    msg.insert(msg.end(), counterStr.begin(), counterStr.end());

    *_aidl_return = msg;
    return binder::Status::ok();
  }
};

int main() {
  // Initialize the binder
  android::ProcessState::initWithDriver("/dev/vndbinder");
  sp<ProcessState> proc(ProcessState::self());
  sp<IServiceManager> sm = defaultServiceManager();

  // Create an instance of the service
  sp<MyService> service = new MyService();

  // Register the service with the Service Manager
  sm->addService(String16("com.example.IMyInterface"), service);

  // Start the thread pool
  ProcessState::self()->startThreadPool();
  IPCThreadState::self()->joinThreadPool();

  return 0;
}
