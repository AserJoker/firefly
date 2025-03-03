#include "script/engine/JSNativeFunction.hpp"
#include "script/engine/JSContext.hpp"
JSNativeFunction::JSNativeFunction(
    JSAllocator *allocator, const std::wstring &name, const JS_NATIVE &native,
    const std::unordered_map<std::wstring, JSAtom *> &closure)
    : JSCallable(allocator, name, closure), _native(native) {}

JSValue *JSNativeFunction::call(JSContext *ctx, JSValue *self,
                                const std::vector<JSValue *> args) {
  return _native(ctx, self, args);
}