#include "script/engine/JSUninitializeType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
JSUninitializeType::JSUninitializeType(JSAllocator *allocator)
    : JSType(allocator) {}
const wchar_t *JSUninitializeType::getTypeName() const {
  return L"uninitialized";
}

JSValue *JSUninitializeType::toString(JSContext *ctx, JSValue *value) const {
  return nullptr;
}

JSValue *JSUninitializeType::toNumber(JSContext *ctx, JSValue *value) const {
  return nullptr;
};

JSValue *JSUninitializeType::toBoolean(JSContext *ctx, JSValue *value) const {
  return nullptr;
};

JSValue *JSUninitializeType::clone(JSContext *ctx, JSValue *value) const {
  return nullptr;
}

JSValue *JSUninitializeType::pack(JSContext *ctx, JSValue *value) const {
  return value;
};

JSValue *JSUninitializeType::equal(JSContext *ctx, JSValue *value,
                                   JSValue *another) const {
  return ctx->createBoolean(another->isTypeof<JSUninitializeType>());
}