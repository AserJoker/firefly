#include "script/runtime/JSFunctionConstructor.hpp"
#include "script/engine/JSExceptionType.hpp"

JSValue *JSFunctionConstructor::constructor(JSContext *ctx, JSValue *self,
                                            std::vector<JSValue *> args) {
  return nullptr;
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
  err =
      ctx->setField(ctx->getGlobal(), ctx->createString(L"Function"), Function);
  if (err) {
    return err;
  }
  return nullptr;
}