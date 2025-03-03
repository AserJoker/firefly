#pragma once

#include "JSBase.hpp"
class JSUndefined : public JSBase {
public:
  JSUndefined(JSAllocator *allocator) : JSBase(allocator, JS_TYPE::UNDEFINED) {}
  JSBase *toString() override;
  JSBase *clone() override;
};