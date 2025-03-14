#include "script/engine/JSContext.hpp"
#include "script/engine/JSInfinity.hpp"
#include "script/engine/JSInfinityType.hpp"
#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"

JSInfinityType::JSInfinityType(JSAllocator *allocator)
    : JSNumberType(allocator) {}

JSValue *JSInfinityType::toNumber(JSContext *ctx, JSValue *value) const {
  return value;
}

JSValue *JSInfinityType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(true);
}

JSValue *JSInfinityType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createInfinity(
      value->getData()->cast<JSInfinity>()->isNegative());
}

JSValue *JSInfinityType::toString(JSContext *ctx, JSValue *value) const {
  auto inf = value->getData()->cast<JSInfinity>();
  if (inf->isNegative()) {
    return ctx->createString(L"-Infinity");
  } else {
    return ctx->createString(L"Infinity");
  }
};

JSValue *JSInfinityType::unaryNegation(JSContext *ctx, JSValue *value) const {
  return ctx->createInfinity(
      !value->getData()->cast<JSInfinity>()->isNegative());
};

JSValue *JSInfinityType::unaryPlus(JSContext *ctx, JSValue *value) const {
  return value;
};