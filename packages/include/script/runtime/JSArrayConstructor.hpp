#pragma once
#include "script/engine/JSContext.hpp"
class JSArrayConstructor {
private:
  class Iterator {
  public:
    static JS_CFUNCTION(next);

    static JS_CFUNCTION(iterator);
  };

  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

  static JSValue *toString(JSContext *ctx, JSValue *self,
                           std::vector<JSValue *> args);

  static JS_CFUNCTION(iterator);

public:
  static JSValue *initialize(JSContext *ctx);
};