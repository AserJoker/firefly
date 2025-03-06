#include "script/engine/JSStringType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSString.hpp"
#include "script/engine/JSType.hpp"
#include "script/util/JSAllocator.hpp"
#include <string>
JSStringType::JSStringType(JSAllocator *allocator) : JSType(allocator, 2) {}

const wchar_t *JSStringType::getTypeName() const { return L"string"; }

JSValue *JSStringType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createString(value->getData()->cast<JSString>()->getValue());
}

JSValue *JSStringType::toNumber(JSContext *ctx, JSValue *value) const {
  auto str = ctx->checkedString(value);
  if ((str[0] >= '0' && str[0] <= '9') ||
      (str[0] == '.' && str[1] >= '0' && str[1] <= '9')) {
    return ctx->createNumber(std::stod(str));
  } else {
    return ctx->createNaN();
  }
};

JSValue *JSStringType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(!ctx->checkedString(value).empty());
};

JSValue *JSStringType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createString(value->getData()->cast<JSString>()->getValue());
}
JSValue *JSStringType::pack(JSContext *ctx, JSValue *value) const {
  return ctx->createException(JSException::TYPE::INTERNAL, L"not implement");
};
JSValue *JSStringType::equal(JSContext *ctx, JSValue *value,
                             JSValue *another) const {
  return ctx->createBoolean(ctx->checkedString(ctx->toString(value)) ==
                            ctx->checkedString(ctx->toString(another)));
}