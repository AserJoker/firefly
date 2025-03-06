#include "script/runtime/JSObjectConstructor.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSValue.hpp"
JSValue *JSObjectConstructor::constructor(JSContext *ctx, JSValue *self,
                                          std::vector<JSValue *> args) {
  return nullptr;
}

JSValue *JSObjectConstructor::initialize(JSContext *ctx) {
  auto Object = ctx->createNativeFunction(&constructor, L"Object");
  auto Function = ctx->getGlobal(L"Function");
  auto prototype = ctx->getField(Object, ctx->createString(L"prototype"));
  if (prototype->isTypeof<JSExceptionType>()) {
    return prototype;
  }
  auto err = ctx->defineProperty(prototype, ctx->createString(L"constructor"),
                                 Object, true, false, true);
  if (err) {
    return err;
  }
  auto funcPrototype = ctx->getField(Function, ctx->createString(L"prototype"));

  funcPrototype->getData()->cast<JSObject>()->setPrototype(
      prototype->getAtom());

  funcPrototype->getAtom()->addChild(prototype->getAtom());

  err = ctx->setField(ctx->getGlobal(), ctx->createString(L"Object"), Object);
  
  if (err) {
    return err;
  }
  return nullptr;
}