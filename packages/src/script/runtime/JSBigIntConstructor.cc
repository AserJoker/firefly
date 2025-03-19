#include "script/runtime/JSBigIntConstructor.hpp"
#include "script/engine/JSBigIntType.hpp"
#include "script/engine/JSBooleanType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/runtime/JSBigIntConstructor.hpp"
#include <vector>

JSValue *JSBigIntConstructor::constructor(JSContext *ctx, JSValue *self,
                                          std::vector<JSValue *> args) {
  JSValue *value = nullptr;
  if (args.empty()) {
    value = ctx->createUndefined();
  } else {
    value = args[0];
  }
  if (value->isTypeof<JSNumberType>() || value->isTypeof<JSBooleanType>()) {
    value = ctx->toNumber(value);
    CHECK(ctx, value);
    value = ctx->createBigInt(ctx->checkedNumber(value));
  }
  if (!value->isTypeof<JSBigIntType>()) {
    value = ctx->toString(value);
    CHECK(ctx, value);
    value = ctx->createBigInt(ctx->checkedString(value));
  }
  if (!self) {
    return value;
  }
  ctx->setMetadata(self, L"primitive", ctx->clone(value));
  return ctx->createUndefined();
}
JSValue *JSBigIntConstructor::toString(JSContext *ctx, JSValue *self,
                                       std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"BigInt.prototype.valueOf requires that 'this' be a BigInt");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getBigIntConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"BigInt.prototype.valueOf requires that 'this' be a BigInt");
  }

  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"BigInt.prototype.valueOf requires that 'this' be a BigInt");
  }
  return ctx->toString(value);
}

JSValue *JSBigIntConstructor::valueOf(JSContext *ctx, JSValue *self,
                                      std::vector<JSValue *> args) {
  if (!self->isTypeof<JSObjectType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"BigInt.prototype.valueOf requires that 'this' be a BigInt");
  }
  auto constructor = ctx->getConstructorOf(self);
  if (constructor->getData() != ctx->getBigIntConstructor()->getData()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"BigInt.prototype.valueOf requires that 'this' be a BigInt");
  }
  auto value = ctx->getMetadata(self, L"primitive");
  if (!value) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        L"BigInt.prototype.valueOf requires that 'this' be a BigInt");
  }
  return ctx->clone(value);
}
JSValue *JSBigIntConstructor::initialize(JSContext *ctx) {
  auto BigInt = ctx->createNativeFunction(constructor, L"BigInt");
  CHECK(ctx, BigInt);
  auto prototype = ctx->getField(BigInt, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto err = ctx->setField(prototype, ctx->createString(L"toString"),
                           ctx->createNativeFunction(toString, L"toString"));
  CHECK(ctx, err);
  err = ctx->setField(prototype, ctx->createString(L"valueOf"),
                      ctx->createNativeFunction(valueOf, L"valueOf"));
  CHECK(ctx, err);
  auto global = ctx->getGlobal();
  err = ctx->setField(global, ctx->createString(L"BigInt"), BigInt);
  CHECK(ctx, err);
  return BigInt;
}