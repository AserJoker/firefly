#include "script/engine/JSSymbolType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSSymbol.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
#include <format>
JSSymbolType::JSSymbolType(JSAllocator *allocator) : JSType(allocator, 5) {}
const wchar_t *JSSymbolType::getTypeName() const { return L"symbol"; }
JSValue *JSSymbolType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createString(std::format(
      L"Symbol({})", value->getData()->cast<JSSymbol>()->getDescription()));
}

JSValue *JSSymbolType::toNumber(JSContext *ctx, JSValue *value) const {
  return ctx->createNaN();
};

JSValue *JSSymbolType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(true);
};

JSValue *JSSymbolType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createSymbol(
      value->getData()->cast<JSSymbol>()->getDescription());
}
JSValue *JSSymbolType::pack(JSContext *ctx, JSValue *value) const {
  return ctx->createException(JSException::TYPE::INTERNAL, L"not implement");
};
JSValue *JSSymbolType::equal(JSContext *ctx, JSValue *value,
                             JSValue *another) const {
  return ctx->createBoolean(value->getAtom() == another->getAtom());
}