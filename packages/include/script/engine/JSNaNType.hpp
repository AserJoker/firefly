#pragma once
#include "script/engine/JSNumberType.hpp"
#include "script/util/JSAllocator.hpp"
class JSNaNType : public JSNumberType {
public:
  JSNaNType(JSAllocator *allocator);

  JSValue *toString(JSContext *ctx, JSValue *value) const override;
  
  JSValue *equal(JSContext *ctx, JSValue *value,
                 JSValue *another) const override;
};