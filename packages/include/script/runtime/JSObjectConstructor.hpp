#pragma once
#include "script/engine/JSContext.hpp"
class JSObjectConstructor {
private:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

public:
  static JSValue *initialize(JSContext *ctx);
};