#include "script/runtime/JSArrayConstructor.hpp"
#include "script/engine/JSArray.hpp"
#include "script/engine/JSValue.hpp"
JSValue *JSArrayConstructor::initialize(JSContext *ctx) {
  auto Array = ctx->createNativeFunction(constructor, L"Array");
  CHECK(ctx, Array);
  auto prototype = ctx->getField(Array, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto toStringFn = ctx->createNativeFunction(toString, L"toString");
  CHECK(ctx, toStringFn);
  auto err =
      ctx->setField(prototype, ctx->createString(L"toString"), toStringFn);
  CHECK(ctx, err);
  auto global = ctx->getGlobal();
  err = ctx->setField(global, ctx->createString(L"Array"), Array);
  CHECK(ctx, err);
  return Array;
}
JSValue *JSArrayConstructor::constructor(JSContext *ctx, JSValue *self,
                                         std::vector<JSValue *> args) {
  return ctx->createUndefined();
}

JSValue *JSArrayConstructor::toString(JSContext *ctx, JSValue *self,
                                      std::vector<JSValue *> args) {
  auto arr = self->getData()->cast<JSArray>();
  auto &items = arr->getItems();
  std::wstring result;
  for (size_t index = 0; index < arr->getLength(); index++) {
    if (items.contains(index)) {
      auto str = ctx->toString(ctx->createValue(items.at(index)));
      CHECK(ctx, str);
      result += ctx->checkedString(str);
    }
    if (index != arr->getLength() - 1) {
      result += L",";
    }
  }
  return ctx->createString(result);
}