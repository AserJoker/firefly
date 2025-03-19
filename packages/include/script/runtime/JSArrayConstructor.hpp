#pragma once
#include "script/engine/JSContext.hpp"
class JSArrayConstructor {
private:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

  static JSValue *toString(JSContext *ctx, JSValue *self,
                           std::vector<JSValue *> args);

public:
  static JSValue *initialize(JSContext *ctx);
};