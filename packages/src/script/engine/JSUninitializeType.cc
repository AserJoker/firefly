#include "script/engine/JSUninitializeType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
JSUninitializeType::JSUninitializeType(JSAllocator *allocator)
    : JSType(allocator, 0) {}
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
  return ctx->createException(JSException::TYPE::INTERNAL, L"not implement");
};

JSValue *JSUninitializeType::equal(JSContext *ctx, JSValue *value,
                                   JSValue *another) const {
  return ctx->createBoolean(another->isTypeof<JSUninitializeType>());
}