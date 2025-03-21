#include "script/engine/JSCallable.hpp"
JSCallable::JSCallable(
    JSAllocator *allocator, const std::wstring &name,
    const std::unordered_map<std::wstring, JSAtom *> &closure, JSType *type)
    : JSObject(allocator, type), _closure(closure), _name(name),
      _globalContext(false), _self(nullptr), _clazz(nullptr){};

void JSCallable::setSelf(JSAtom *self) { _self = self; }

JSAtom *JSCallable::getSelf() { return _self; }

void JSCallable::setClass(JSAtom *clazz) { _clazz = clazz; }

JSAtom *JSCallable::getClass() { return _clazz; }