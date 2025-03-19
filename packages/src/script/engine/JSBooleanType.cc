#include "script/engine/JSBooleanType.hpp"
#include "script/engine/JSBoolean.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
JSBooleanType::JSBooleanType(JSAllocator *allocator) : JSType(allocator) {}

const wchar_t *JSBooleanType::getTypeName() const { return L"boolean"; }

JSValue *JSBooleanType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createString(
      value->getData()->cast<JSBoolean>()->getValue() ? L"true" : L"false");
}

JSValue *JSBooleanType::toNumber(JSContext *ctx, JSValue *value) const {
  return ctx->createNumber(value->getData()->cast<JSBoolean>()->getValue() ? 1
                                                                           : 0);
};

JSValue *JSBooleanType::toBoolean(JSContext *ctx, JSValue *value) const {
  return value;
};

JSValue *JSBooleanType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(value->getData()->cast<JSBoolean>()->getValue());
}

JSValue *JSBooleanType::pack(JSContext *ctx, JSValue *value) const {
  auto Boolean = ctx->getBooleanConstructor();
  return ctx->construct(Boolean, {value});
};

JSValue *JSBooleanType::equal(JSContext *ctx, JSValue *value,
                              JSValue *another) const {
  return ctx->createBoolean(ctx->checkedBoolean(ctx->toBoolean(value)) ==
                            ctx->checkedBoolean(ctx->toBoolean(another)));
}