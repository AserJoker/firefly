#include "script/engine/JSNullType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
JSNullType::JSNullType(JSAllocator *allocator) : JSType(allocator) {}
const wchar_t *JSNullType::getTypeName() const { return L"object"; }

JSValue *JSNullType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createString(L"null");
}

JSValue *JSNullType::toNumber(JSContext *ctx, JSValue *value) const {
  return ctx->createNumber(0);
};

JSValue *JSNullType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(false);
};

JSValue *JSNullType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createNull();
}
JSValue *JSNullType::pack(JSContext *ctx, JSValue *value) const {
  return value;
};
JSValue *JSNullType::equal(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBoolean(another->isTypeof<JSNullType>());
}