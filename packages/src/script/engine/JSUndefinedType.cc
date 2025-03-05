#include "script/engine/JSUndefinedType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSType.hpp"
#include "script/util/JSAllocator.hpp"
JSUndefinedType::JSUndefinedType(JSAllocator *allocator)
    : JSType(allocator, 0) {}
const wchar_t *JSUndefinedType::getTypeName() const { return L"undefined"; }
JSValue *JSUndefinedType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createUndefined();
}
JSValue *JSUndefinedType::toNumber(JSContext *ctx, JSValue *value) const {
  return ctx->createNaN();
};

JSValue *JSUndefinedType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(false);
};
JSValue *JSUndefinedType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createUndefined();
}
JSValue *JSUndefinedType::pack(JSContext *ctx, JSValue *value) const {
  return ctx->createException(JSException::TYPE::INTERNAL, L"not implement");
};
JSValue *JSUndefinedType::equal(JSContext *ctx, JSValue *value,
                                JSValue *another) const {
  return ctx->createBoolean(another->isTypeof<JSUndefinedType>());
}