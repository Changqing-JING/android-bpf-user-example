/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: aidl-cpp com/example/IMyInterface.aidl ./ ./com/example/IMyInterface.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IInterface.h>
#include <com/example/IMyInterface.h>
#include <binder/Delegate.h>
#include <com/example/BnMyInterface.h>


namespace com {
namespace example {
class LIBBINDER_EXPORTED BnMyInterface : public ::android::BnInterface<IMyInterface> {
public:
  static constexpr uint32_t TRANSACTION_getMessage = ::android::IBinder::FIRST_CALL_TRANSACTION + 0;
  explicit BnMyInterface();
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) override;
};  // class BnMyInterface

class LIBBINDER_EXPORTED IMyInterfaceDelegator : public BnMyInterface {
public:
  explicit IMyInterfaceDelegator(const ::android::sp<IMyInterface> &impl) : _aidl_delegate(impl) {}

  ::android::sp<IMyInterface> getImpl() { return _aidl_delegate; }
  ::android::binder::Status getMessage(::std::vector<uint8_t>* _aidl_return) override {
    return _aidl_delegate->getMessage(_aidl_return);
  }
private:
  ::android::sp<IMyInterface> _aidl_delegate;
};  // class IMyInterfaceDelegator
}  // namespace example
}  // namespace com
