#pragma once
#include "script/engine/JSContext.hpp"
#include "script/engine/JSType.hpp"
#include "script/util/JSAllocator.hpp"

class JSInterruptType : public JSType {
public:
  JSInterruptType(JSAllocator *allocator) : JSType(allocator) {}
  
  JSValue *toString(JSContext *ctx, JSValue *value) const override {
    return ctx->createString(L"interrupt");
  };

  JSValue *toNumber(JSContext *ctx, JSValue *value) const override {
    return ctx->createNaN();
  };

  JSValue *toBoolean(JSContext *ctx, JSValue *value) const override {
    return ctx->createBoolean(false);
  };

  JSValue *clone(JSContext *ctx, JSValue *value) const override {
    return value;
  };

  JSValue *pack(JSContext *ctx, JSValue *value) const override {
    return value;
  };

  JSValue *equal(JSContext *ctx, JSValue *value,
                 JSValue *another) const override {
    return ctx->createBoolean(value->getData() == another->getData());
  };
};