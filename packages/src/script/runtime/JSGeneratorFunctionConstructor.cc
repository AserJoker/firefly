#include "script/runtime/JSGeneratorFunctionConstructor.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>
JSValue *
JSGeneratorFunctionConstructor::constructor(JSContext *ctx, JSValue *value,
                                            std::vector<JSValue *> args) {
  return ctx->createUndefined();
}

JSValue *JSGeneratorFunctionConstructor::initialize(JSContext *ctx) {
  auto GeneratorFunction =
      ctx->createNativeFunction(&constructor, L"GeneratorFunction");
  auto Function = ctx->getFunctionConstructor();
  auto prototype =
      ctx->getField(GeneratorFunction, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto fnPrototype = ctx->getField(Function, ctx->createString(L"prototype"));
  CHECK(ctx, fnPrototype);
  auto err = ctx->setPrototype(prototype, fnPrototype);
  CHECK(ctx, err);
  return GeneratorFunction;
}