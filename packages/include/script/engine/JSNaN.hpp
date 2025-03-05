#pragma once
#include "JSBase.hpp"
#include "script/util/JSAllocator.hpp"
class JSNaN : public JSBase {
public:
  JSNaN(JSAllocator *allocator);
};