#pragma once
#include "script/engine/JSContext.hpp"
class JSGeneratorFunctionConstructor {
private:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

public:
  static JSValue *initialize(JSContext *ctx);
};