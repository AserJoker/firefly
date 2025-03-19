#pragma once
#include "script/engine/JSContext.hpp"
class JSBooleanConstructor {
public:
private:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

  static JSValue *toString(JSContext *ctx, JSValue *self,
                           std::vector<JSValue *> args);

  static JSValue *valueOf(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

public:
  static JSValue *initialize(JSContext *ctx);
};