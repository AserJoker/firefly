#include "script/engine/JSNaNType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"

JSNaNType::JSNaNType(JSAllocator *allocator) : JSNumberType(allocator) {
  setPriority(6);
}

JSValue *JSNaNType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createString(L"NaN");
};

JSValue *JSNaNType::equal(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  return ctx->createBoolean(false);
}