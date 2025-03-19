#include "script/runtime/JSBooleanConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>
JSValue *JSBooleanConstructor::constructor(JSContext *ctx, JSValue *self,
                                           std::vector<JSValue *> args) {
  JSValue *value = nullptr;
  if (args.empty()) {
    value = ctx->createUndefined();
  } else {
    value = args[0];
  }
  value = ctx->toBoolean(value);
  CHECK(ctx, value);
  if (!self) {
    return value;
  }
  ctx->setMetadata(self, L"primitive", ctx->clone(value));
  return ctx->createUndefined();
}
JSValue *JSBooleanConstructor::toString(JSContext *ctx, JSValue *self,
                                        std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Boolean.prototype.valueOf requires that 'this' be a Boolean");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getBooleanConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Boolean.prototype.valueOf requires that 'this' be a Boolean");
  }

  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Boolean.prototype.valueOf requires that 'this' be a Boolean");
  }
  return ctx->toString(value);
}

JSValue *JSBooleanConstructor::valueOf(JSContext *ctx, JSValue *self,
                                       std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Boolean.prototype.valueOf requires that 'this' be a Boolean");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getBooleanConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Boolean.prototype.valueOf requires that 'this' be a Boolean");
  }
  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Boolean.prototype.valueOf requires that 'this' be a Boolean");
  }
  return ctx->clone(value);
}
JSValue *JSBooleanConstructor::initialize(JSContext *ctx) {
  auto Boolean = ctx->createNativeFunction(constructor, L"Boolean");
  CHECK(ctx, Boolean);
  auto prototype = ctx->getField(Boolean, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto err = ctx->setField(prototype, ctx->createString(L"toString"),
                           ctx->createNativeFunction(toString, L"toString"));
  CHECK(ctx, err);
  err = ctx->setField(prototype, ctx->createString(L"valueOf"),
                      ctx->createNativeFunction(valueOf, L"valueOf"));
  CHECK(ctx, err);
  auto global = ctx->getGlobal();
  err = ctx->setField(global, ctx->createString(L"Boolean"), Boolean);
  CHECK(ctx, err);
  return Boolean;
}