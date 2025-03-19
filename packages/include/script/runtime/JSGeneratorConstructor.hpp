#pragma once
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>
class JSGeneratorConstructor {
private:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

  static JS_CFUNCTION(next);

  static JS_CFUNCTION(throw_);

  static JS_CFUNCTION(return_);

  static JS_CFUNCTION(iterator);

public:
  static JSValue *initialize(JSContext *ctx);
};