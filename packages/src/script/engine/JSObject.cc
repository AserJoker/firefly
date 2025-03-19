#include "script/engine/JSObject.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/util/JSSingleton.hpp"
JSObject::JSObject(JSAllocator *allocator, JSType *type)
    : JSBase(allocator, type == nullptr
                            ? JSSingleton::instance<JSObjectType>(allocator)
                            : type),
      _sealed(false), _frozen(false), _extensible(true), _prototype(nullptr),
      _constructor(nullptr) {}
