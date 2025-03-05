#pragma once
#include "JSCallableType.hpp"
class JSNativeFunctionType : public JSCallableType {
public:
  JSNativeFunctionType(JSAllocator *allocator);
};