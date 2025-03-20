#include "script/runtime/JSArrayConstructor.hpp"
#include "script/engine/JSArray.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSNumberType.hpp"
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
  err = ctx->setField(prototype,
                      ctx->getField(ctx->getSymbolConstructor(),
                                    ctx->createString(L"iterator")),
                      ctx->createNativeFunction(&iterator, L"Symbol.iterator"));
  CHECK(ctx, err);
  auto global = ctx->getGlobal();
  err = ctx->setField(global, ctx->createString(L"Array"), Array);
  CHECK(ctx, err);
  return Array;
}
JSValue *JSArrayConstructor::constructor(JSContext *ctx, JSValue *self,
                                         std::vector<JSValue *> args) {
  auto array =
      ctx->getScope()->createValue(ctx->getAllocator()->create<JSArray>());
  auto constructor = ctx->getArrayConstructor();
  auto prototype = ctx->getField(constructor, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto err = ctx->setPrototype(array, prototype);
  CHECK(ctx, err);
  err = ctx->setConstructor(array, constructor);
  CHECK(ctx, err);
  return array;
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

JS_CFUNCTION(JSArrayConstructor::iterator) {
  auto iterator = ctx->createObject();
  auto err = ctx->setField(
      iterator,
      ctx->getField(ctx->getSymbolConstructor(),
                    ctx->createString(L"iterator")),
      ctx->createNativeFunction(&JSArrayConstructor::Iterator::iterator,
                                L"[Symbol.iterator]"));
  CHECK(ctx, err);
  err = ctx->setField(
      iterator, ctx->createString(L"next"),
      ctx->createNativeFunction(&JSArrayConstructor::Iterator::next, L"next"));
  CHECK(ctx, err);
  err = ctx->setField(iterator,
                      ctx->getField(ctx->getSymbolConstructor(),
                                    ctx->createString(L"toStringTag")),
                      ctx->createString(L"Array Iterator"));
  CHECK(ctx, err);
  ctx->setMetadata(iterator, L"array", self);
  ctx->setMetadata(iterator, L"index", ctx->createNumber(0));
  return iterator;
}

JS_CFUNCTION(JSArrayConstructor::Iterator::iterator) { return self; }

JS_CFUNCTION(JSArrayConstructor::Iterator::next) {
  auto array = ctx->getMetadata(self, L"array");
  auto idx = ctx->getMetadata(self, L"index");
  if (!array || !array->isTypeof<JSArray>() || !idx ||
      !idx->isTypeof<JSNumberType>()) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        std::format(L"Method Array Iterator.prototype.next called on "
                    L"incompatible receiver {}",
                    ctx->checkedString(ctx->toString(ctx->pack(self)))));
  }
  auto arr = array->getData()->cast<JSArray>();
  auto length = arr->getLength();
  JSValue *value = nullptr;
  JSValue *done = nullptr;
  if (ctx->checkedNumber(idx) < length) {
    value = ctx->getField(array, idx);
    ctx->inc(idx);
    done = ctx->createBoolean(false);
  } else {
    value = ctx->createUndefined();
    done = ctx->createBoolean(true);
  }
  auto result = ctx->createObject();
  ctx->setField(result, ctx->createString(L"value"), value);
  ctx->setField(result, ctx->createString(L"done"), done);
  return result;
}