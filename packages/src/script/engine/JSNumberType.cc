#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSNumber.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
JSNumberType::JSNumberType(JSAllocator *allocator) : JSType(allocator, 1) {}

const wchar_t *JSNumberType::getTypeName() const { return L"number"; }

JSValue *JSNumberType::toString(JSContext *ctx, JSValue *value) const {
  auto number = value->getData()->cast<JSNumber>();
  return ctx->createString(std::format(L"{}", number->getValue()));
}

JSValue *JSNumberType::toNumber(JSContext *ctx, JSValue *value) const {
  return value;
}

JSValue *JSNumberType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(value->getData()->cast<JSNumber>()->getValue() !=
                            0);
}

JSValue *JSNumberType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createNumber(value->getData()->cast<JSNumber>()->getValue());
}
JSValue *JSNumberType::pack(JSContext *ctx, JSValue *value) const {
  auto Number = ctx->getField(ctx->getGlobal(), ctx->createString(L"Number"));
  return ctx->construct(Number, {value});
};
JSValue *JSNumberType::equal(JSContext *ctx, JSValue *value,
                             JSValue *another) const {
  return ctx->createBoolean(ctx->checkedNumber(ctx->toNumber(value)) ==
                            ctx->checkedNumber(ctx->toNumber(another)));
}