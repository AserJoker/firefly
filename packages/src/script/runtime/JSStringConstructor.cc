#include "script/runtime/JSStringConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>

JSValue *JSStringConstructor::constructor(JSContext *ctx, JSValue *self,
                                          std::vector<JSValue *> args) {
  JSValue *value = nullptr;
  if (args.empty()) {
    value = ctx->createUndefined();
  } else {
    value = args[0];
  }
  value = ctx->toString(value);
  CHECK(ctx, value);
  if (!self) {
    return value;
  }
  ctx->setMetadata(self, L"primitive", ctx->clone(value));
  return ctx->createUndefined();
}
JSValue *JSStringConstructor::toString(JSContext *ctx, JSValue *self,
                                       std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"String.prototype.toString requires that 'this' be a String");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getStringConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"String.prototype.toString requires that 'this' be a String");
  }

  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"String.prototype.toString requires that 'this' be a String");
  }
  return ctx->toString(value);
}

JSValue *JSStringConstructor::valueOf(JSContext *ctx, JSValue *self,
                                      std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"String.prototype.valueOf requires that 'this' be a String");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getStringConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"String.prototype.valueOf requires that 'this' be a String");
  }
  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"String.prototype.valueOf requires that 'this' be a String");
  }
  return ctx->clone(value);
}
JSValue *JSStringConstructor::initialize(JSContext *ctx) {
  auto String = ctx->createNativeFunction(constructor, L"String");
  CHECK(ctx, String);
  auto prototype = ctx->getField(String, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto err = ctx->setField(prototype, ctx->createString(L"toString"),
                           ctx->createNativeFunction(toString, L"toString"));
  CHECK(ctx, err);
  err = ctx->setField(prototype, ctx->createString(L"valueOf"),
                      ctx->createNativeFunction(valueOf, L"valueOf"));
  CHECK(ctx, err);
  auto global = ctx->getGlobal();
  err = ctx->setField(global, ctx->createString(L"String"), String);
  CHECK(ctx, err);
  return String;
}