#pragma once
#include "script/engine/JSObjectType.hpp"
#include "script/util/JSAllocator.hpp"
class JSArrayType : public JSObjectType {
public:
  JSArrayType(JSAllocator *allocator);
  
  JSValue *toString(JSContext *ctx, JSValue *value) const override;
};