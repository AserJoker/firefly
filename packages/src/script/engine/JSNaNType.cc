#include "script/engine/JSNaNType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"

JSNaNType::JSNaNType(JSAllocator *allocator) : JSNumberType(allocator) {}
JSValue *JSNaNType::toNumber(JSContext *ctx, JSValue *value) const {
  return value;
}

JSValue *JSNaNType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(false);
}

JSValue *JSNaNType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createNaN();
}

JSValue *JSNaNType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createString(L"NaN");
};

JSValue *JSNaNType::unaryNegation(JSContext *ctx, JSValue *value) const {
  return value;
};

JSValue *JSNaNType::unaryPlus(JSContext *ctx, JSValue *value) const {
  return value;
};
