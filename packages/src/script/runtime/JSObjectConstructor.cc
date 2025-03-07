#include "script/runtime/JSObjectConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSNullType.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>
JSValue *JSObjectConstructor::constructor(JSContext *ctx, JSValue *self,
                                          std::vector<JSValue *> args) {
  return nullptr;
}
JSValue *JSObjectConstructor::toString(JSContext *ctx, JSValue *value,
                                       std::vector<JSValue *> args) {
  std::wstring name;
  ctx->pushScope();
  auto prototype = ctx->getPrototypeOf(value);
  if (prototype->isTypeof<JSExceptionType>()) {
    return prototype;
  }
  if (prototype->isTypeof<JSNullType>()) {
    name = L"[Object: null prototype]";
  } else {
    auto constructor = ctx->getConstructorOf(value);
    if (ctx->checkedBoolean(ctx->isEqual(
            constructor, ctx->getGlobal(ctx->createString(L"Object"))))) {
      name = L"";
    } else {
      name = constructor->getData()->cast<JSCallable>()->getName();
    }
  }
  if (name.empty()) {
    name = L"{}";
  } else {
    name = name + L" {}";
  }
  ctx->popScope();
  return ctx->createString(name);
}

JSValue *JSObjectConstructor::initialize(JSContext *ctx) {
  auto Object = ctx->createNativeFunction(&constructor, L"Object");
  auto Function = ctx->getGlobal(ctx->createString(L"Function"));
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

  err = ctx->setPrototype(funcPrototype, prototype);
  if (err) {
    return err;
  }

  auto global = ctx->getGlobal();

  err = ctx->setPrototype(global, prototype);
  if (err) {
    return err;
  }

  err = ctx->defineProperty(prototype, ctx->createString(L"toString"),
                            ctx->createNativeFunction(toString, L"toString"),
                            true, false, true);
  if (err) {
    return err;
  }

  err = ctx->setField(global, ctx->createString(L"Object"), Object);

  if (err) {
    return err;
  }
  return nullptr;
}