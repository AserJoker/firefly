#pragma once
#include "JSBase.hpp"
class JSUninitialize : public JSBase {
public:
  JSUninitialize(JSAllocator *allocator)
      : JSBase(allocator, JS_TYPE::UNINITIALIZED) {}
  JSBase *toString() override;
  JSBase *clone() override;
};
