#pragma once
#include "JSCallableType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"
class JSFunctionType : public JSCallableType {
public:
  JSFunctionType(JSAllocator *allocator);

public:
  JSValue *call(JSContext *ctx, JSValue *func, JSValue *self,
                std::vector<JSValue *> args) const override;

  JSValue *setSelf(JSContext *ctx, JSValue *value, JSValue *self);

  JSValue *getSelf(JSContext *ctx, JSValue *value);

  JSValue *setClass(JSContext *ctx, JSValue *value, JSValue *clazz);

  JSValue *getClass(JSContext *ctx, JSValue *value);
};