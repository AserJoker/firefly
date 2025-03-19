#pragma once
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>
class JSObjectConstructor {
private:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

  static JSValue *toString(JSContext *ctx, JSValue *self,
                           std::vector<JSValue *> args);

public:
  static JSValue *initialize(JSContext *ctx);
};