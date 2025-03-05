#pragma once
#include "JSCallableType.hpp"
class JSFunctionType : public JSCallableType {
public:
  JSFunctionType(JSAllocator *allocator);
};