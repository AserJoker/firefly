#include "script/engine/JSNativeFunction.hpp"
#include "script/engine/JSNativeFunctionType.hpp"
#include "script/util/JSSingleton.hpp"

JSNativeFunction::JSNativeFunction(
    JSAllocator *allocator, const std::wstring &name, const JS_NATIVE &native,
    const std::unordered_map<std::wstring, JSAtom *> &closure)
    : JSCallable(allocator, name, closure,
                 JSSingleton::instance<JSNativeFunctionType>(allocator)),
      _native(native) {}
