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
#include <binder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>
#include <vector>

namespace com {
namespace example {
class LIBBINDER_EXPORTED IMyInterfaceDelegator;

class LIBBINDER_EXPORTED IMyInterface : public ::android::IInterface {
public:
  typedef IMyInterfaceDelegator DefaultDelegator;
  DECLARE_META_INTERFACE(MyInterface)
  virtual ::android::binder::Status getMessage(::std::vector<uint8_t>* _aidl_return) = 0;
};  // class IMyInterface

class LIBBINDER_EXPORTED IMyInterfaceDefault : public IMyInterface {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status getMessage(::std::vector<uint8_t>* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class IMyInterfaceDefault
}  // namespace example
}  // namespace com
