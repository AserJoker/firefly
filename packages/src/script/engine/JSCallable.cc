#include "script/engine/JSCallable.hpp"
JSCallable::JSCallable(
    JSAllocator *allocator, const std::wstring &name,
    const std::unordered_map<std::wstring, JSAtom *> &closure, JSType *type)
    : JSObject(allocator, type), _closure(closure), _name(name),
      _globalContext(false){};
