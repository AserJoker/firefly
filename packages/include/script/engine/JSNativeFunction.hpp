#pragma once

#include "JSCallable.hpp"
class JSNativeFunction : public JSCallable {
private:
  JS_NATIVE _native;

public:
  JSNativeFunction(
      JSAllocator *allocator, const std::wstring &name, const JS_NATIVE &native,
      const std::unordered_map<std::wstring, JSAtom *> &closure = {});

public:
  JSValue *call(JSContext *ctx, JSValue *self,
                const std::vector<JSValue *> args) override;
};
