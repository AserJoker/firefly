#pragma once
#include "script/engine/JSNumberType.hpp"
#include "script/util/JSAllocator.hpp"
class JSNaNType : public JSNumberType {
public:
  JSNaNType(JSAllocator *allocator);

  JSValue *toNumber(JSContext *ctx, JSValue *value) const override;

  JSValue *toBoolean(JSContext *ctx, JSValue *value) const override;

  JSValue *clone(JSContext *ctx, JSValue *value) const override;

  JSValue *toString(JSContext *ctx, JSValue *value) const override;

  JSValue *unaryNegation(JSContext *ctx, JSValue *value) const override;

  JSValue *unaryPlus(JSContext *ctx, JSValue *value) const override;
};