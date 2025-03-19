#include "script/engine/JSSymbolType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSSymbol.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
JSSymbolType::JSSymbolType(JSAllocator *allocator) : JSType(allocator) {}
const wchar_t *JSSymbolType::getTypeName() const { return L"symbol"; }
JSValue *JSSymbolType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createException(JSException::TYPE::TYPE,
                              L"Cannot convert Symbol to string");
}

JSValue *JSSymbolType::toNumber(JSContext *ctx, JSValue *value) const {
  return ctx->createException(JSException::TYPE::TYPE,
                              L"Cannot convert Symbol to number");
};

JSValue *JSSymbolType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(true);
};

JSValue *JSSymbolType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createSymbol(
      value->getData()->cast<JSSymbol>()->getDescription());
}
JSValue *JSSymbolType::pack(JSContext *ctx, JSValue *value) const {
  auto Symbol = ctx->getSymbolConstructor();
  auto prototype = ctx->getField(Symbol, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto object = ctx->createObject(prototype);
  CHECK(ctx, object);
  auto err = ctx->defineProperty(object, ctx->createString(L"constructor"),
                                 Symbol, true, false, true);
  CHECK(ctx, err);
  err = ctx->setConstructor(object, Symbol);
  CHECK(ctx, err);
  ctx->setMetadata(object, L"primitive", value);
  auto fieldname = ctx->createString(L"description");
  auto description = value->getData()->cast<JSSymbol>()->getDescription();
  err = ctx->defineProperty(prototype, fieldname,
                            ctx->createString(description), true, false, false);
  CHECK(ctx, err);
  return object;
};
JSValue *JSSymbolType::equal(JSContext *ctx, JSValue *value,
                             JSValue *another) const {
  return ctx->createBoolean(value->getData() == another->getData());
}