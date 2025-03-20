#include "script/runtime/JSObjectConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSNullType.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSStringType.hpp"
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
    auto toStringTag =
        ctx->getField(value, ctx->getField(ctx->getSymbolConstructor(),
                                           ctx->createString(L"toStringTag")));
    CHECK(ctx, toStringTag);
    if (toStringTag->isTypeof<JSStringType>()) {
      name = ctx->checkedString(toStringTag);
    } else {
      auto constructor = ctx->getConstructorOf(value);
      if (constructor->getData() == ctx->getObjectConstructor()->getData()) {
        name = L"";
      } else {
        name = constructor->getData()->cast<JSCallable>()->getName();
      }
    }
  }
  if (name.empty()) {
    name = L"Object";
  }
  name = std::format(L"[object {}]", name);
  ctx->popScope();
  return ctx->createString(name);
}

JSValue *JSObjectConstructor::initialize(JSContext *ctx) {
  auto Object = ctx->createNativeFunction(&constructor, L"Object");
  CHECK(ctx, Object);
  auto global = ctx->getGlobal();
  auto Function = ctx->getField(global, ctx->createString(L"Function"));
  CHECK(ctx, Function);
  auto prototype = ctx->getField(Object, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto err = ctx->defineProperty(prototype, ctx->createString(L"constructor"),
                                 Object, true, false, true);
  CHECK(ctx, err);
  auto funcPrototype = ctx->getField(Function, ctx->createString(L"prototype"));
  CHECK(ctx, funcPrototype);
  err = ctx->setPrototype(funcPrototype, prototype);
  CHECK(ctx, err);
  err = ctx->setPrototype(global, prototype);
  CHECK(ctx, err);
  err = ctx->defineProperty(prototype, ctx->createString(L"toString"),
                            ctx->createNativeFunction(toString, L"toString"),
                            true, false, true);
  CHECK(ctx, err);
  err = ctx->setField(global, ctx->createString(L"Object"), Object);
  CHECK(ctx, err);
  return Object;
}