#include "script/runtime/JSFunctionConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSExceptionType.hpp"
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
  if (Function->isTypeof<JSExceptionType>()) {
    return Function;
  }
  auto err = ctx->defineProperty(Function, ctx->createString(L"constructor"),
                                 Function, true, false, true);
  if (err) {
    return err;
  }
  auto prototype = ctx->getField(Function, ctx->createString(L"prototype"));
  if (prototype->isTypeof<JSExceptionType>()) {
    return prototype;
  }
  err = ctx->defineProperty(prototype, ctx->createString(L"constructor"),
                            Function, true, false, true);
  if (err) {
    return err;
  }

  err = ctx->defineProperty(prototype, ctx->createString(L"toString"),
                            ctx->createNativeFunction(toString, L"toString"),
                            true, false, true);
  if (err) {
    return err;
  }
  err =
      ctx->setField(ctx->getGlobal(), ctx->createString(L"Function"), Function);
  if (err) {
    return err;
  }
  return nullptr;
}