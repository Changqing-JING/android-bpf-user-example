/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: aidl-cpp com/example/IMyInterface.aidl ./ ./com/example/IMyInterface.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/Errors.h>
#include <com/example/IMyInterface.h>

namespace com {
namespace example {
class LIBBINDER_EXPORTED BpMyInterface : public ::android::BpInterface<IMyInterface> {
public:
  explicit BpMyInterface(const ::android::sp<::android::IBinder>& _aidl_impl);
  virtual ~BpMyInterface() = default;
  ::android::binder::Status getMessage(::std::vector<uint8_t>* _aidl_return) override;
};  // class BpMyInterface
}  // namespace example
}  // namespace com
