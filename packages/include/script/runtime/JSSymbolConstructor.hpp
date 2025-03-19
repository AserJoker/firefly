#pragma once
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"
#include <vector>
class JSSymbolConstructor {
private:
  static JSValue *toString(JSContext *ctx, JSValue *self,
                           std::vector<JSValue *> args);

  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

  static JSValue *toPrimitive(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args);

  static JSValue *for_(JSContext *ctx, JSValue *self,
                       std::vector<JSValue *> args);

  static JSValue *keyFor(JSContext *ctx, JSValue *self,
                         std::vector<JSValue *> args);

public:
  static JSValue *initialize(JSContext *ctx);
};