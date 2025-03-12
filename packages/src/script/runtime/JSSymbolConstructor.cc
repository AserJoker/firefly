#include "script/runtime/JSSymbolConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>
JSValue *JSSymbolConstructor::toString(JSContext *ctx, JSValue *self,
                                       std::vector<JSValue *> args) {
  auto description = ctx->getField(self, ctx->createString(L"description"));
  CHECK(ctx, description);
  auto str = ctx->checkedString(description);
  return ctx->createString(std::format(L"Symbol({})", str));
}

JSValue *JSSymbolConstructor::constructor(JSContext *ctx, JSValue *self,
                                          std::vector<JSValue *> args) {
  if (!args.empty()) {
    auto arg = ctx->toString(args[0]);
    CHECK(ctx, arg);
    return ctx->createSymbol(ctx->checkedString(arg));
  }
  return ctx->createSymbol();
}
JSValue *JSSymbolConstructor::toPrimitive(JSContext *ctx, JSValue *self,
                                          std::vector<JSValue *> args) {
  return ctx->getMetadata(self, L"primitive");
}

JSValue *JSSymbolConstructor::initialize(JSContext *ctx) {
  auto Symbol = ctx->createNativeFunction(&constructor, L"Symbol");
  CHECK(ctx, Symbol);
  auto prototype = ctx->getField(Symbol, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);

  auto err = ctx->setField(prototype, ctx->createString(L"toString"),
                           ctx->createNativeFunction(toString, L"toString"));
  CHECK(ctx, err);

  err = ctx->setField(Symbol, ctx->createString(L"asyncIterator"),
                      ctx->createSymbol(L"asyncIterator"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"hasInstance"),
                      ctx->createSymbol(L"hasInstance"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"isConcatSpreadable"),
                      ctx->createSymbol(L"isConcatSpreadable"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"iterator"),
                      ctx->createSymbol(L"iterator"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"match"),
                      ctx->createSymbol(L"match"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"matchAll"),
                      ctx->createSymbol(L"matchAll"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"replace"),
                      ctx->createSymbol(L"replace"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"search"),
                      ctx->createSymbol(L"search"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"species"),
                      ctx->createSymbol(L"species"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"toPrimitive"),
                      ctx->createSymbol(L"toPrimitive"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"toStringTag"),
                      ctx->createSymbol(L"toStringTag"));
  CHECK(ctx, err);
  err = ctx->setField(Symbol, ctx->createString(L"toStringTag"),
                      ctx->createSymbol(L"toStringTag"));
  CHECK(ctx, err);
  err = ctx->setField(
      prototype, ctx->getField(Symbol, ctx->createString(L"toPrimitive")),
      ctx->createNativeFunction(toPrimitive, L"[Symbol.toPrimitive]"));
  CHECK(ctx, err);
  err = ctx->setField(ctx->getGlobal(), ctx->createString(L"Symbol"), Symbol);
  CHECK(ctx, err);
  return nullptr;
};