#include "script/engine/JSBoolean.hpp"
#include "script/engine/JSBooleanType.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"

JSBoolean::JSBoolean(JSAllocator *allocator, bool value)
    : JSBase(allocator, JSSingleton::instance<JSBooleanType>(allocator)),
      _value(value) {}
