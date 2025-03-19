#include "script/runtime/JSFunctionConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"

JSValue *JSFunctionConstructor::constructor(JSContext *ctx, JSValue *self,
                                            std::vector<JSValue *> args) {
  return nullptr;
}

JSValue *JSFunctionConstructor::toString(JSContext *ctx, JSValue *self,
                                         std::vector<JSValue *> args) {
  auto callable = self->getData()->cast<JSCallable>();
  auto name = callable->getName();
  if (name.empty()) {
    name = L"anonymous";
  }
  return ctx->createString(std::format(L"[Function {}]", name));
}

JSValue *JSFunctionConstructor::initialize(JSContext *ctx) {

  auto Function = ctx->createNativeFunction(&constructor, L"Function");
  CHECK(ctx, Function);
  auto err = ctx->defineProperty(Function, ctx->createString(L"constructor"),
                                 Function, true, false, true);
  CHECK(ctx, err);
  auto prototype = ctx->getField(Function, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  err = ctx->defineProperty(prototype, ctx->createString(L"constructor"),
                            Function, true, false, true);
  CHECK(ctx, err);
  err = ctx->defineProperty(prototype, ctx->createString(L"toString"),
                            ctx->createNativeFunction(toString, L"toString"),
                            true, false, true);
  CHECK(ctx, err);
  err =
      ctx->setField(ctx->getGlobal(), ctx->createString(L"Function"), Function);
  CHECK(ctx, err);
  return Function;
}