#include "script/runtime/JSNumberConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>

JSValue *JSNumberConstructor::constructor(JSContext *ctx, JSValue *self,
                                          std::vector<JSValue *> args) {
  JSValue *value = nullptr;
  if (args.empty()) {
    value = ctx->createUndefined();
  } else {
    value = args[0];
  }
  value = ctx->toNumber(value);
  CHECK(ctx, value);
  if (!self) {
    return value;
  }
  ctx->setMetadata(self, L"primitive", ctx->clone(value));
  return ctx->createUndefined();
}
JSValue *JSNumberConstructor::toString(JSContext *ctx, JSValue *self,
                                       std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Number.prototype.valueOf requires that 'this' be a Number");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getNumberConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Number.prototype.valueOf requires that 'this' be a Number");
  }

  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Number.prototype.valueOf requires that 'this' be a Number");
  }
  return ctx->toString(value);
}

JSValue *JSNumberConstructor::valueOf(JSContext *ctx, JSValue *self,
                                      std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Number.prototype.valueOf requires that 'this' be a Number");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getNumberConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Number.prototype.valueOf requires that 'this' be a Number");
  }
  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"Number.prototype.valueOf requires that 'this' be a Number");
  }
  return ctx->clone(value);
}
JSValue *JSNumberConstructor::initialize(JSContext *ctx) {
  auto Number = ctx->createNativeFunction(constructor, L"Number");
  CHECK(ctx, Number);
  auto prototype = ctx->getField(Number, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto err = ctx->setField(prototype, ctx->createString(L"toString"),
                           ctx->createNativeFunction(toString, L"toString"));
  CHECK(ctx, err);
  err = ctx->setField(prototype, ctx->createString(L"valueOf"),
                      ctx->createNativeFunction(valueOf, L"valueOf"));
  CHECK(ctx, err);
  auto global = ctx->getGlobal();
  err = ctx->setField(global, ctx->createString(L"Number"), Number);
  CHECK(ctx, err);
  return Number;
}