#include "script/engine/JSArray.hpp"
#include "script/engine/JSArrayType.hpp"
#include "script/util/JSSingleton.hpp"

JSArray::JSArray(JSAllocator *allocator)
    : JSObject(allocator, JSSingleton::instance<JSArrayType>(allocator)) {}
