#pragma once

#include "JSCallable.hpp"
class JSNativeFunction : public JSCallable {
private:
  JS_NATIVE _native;

public:
  JSNativeFunction(
      JSAllocator *allocator, const std::wstring &name, const JS_NATIVE &native,
      const std::unordered_map<std::wstring, JSAtom *> &closure = {});

  inline const JS_NATIVE &getNative() const { return _native; }

  inline JS_NATIVE &getNative() { return _native; }
};
