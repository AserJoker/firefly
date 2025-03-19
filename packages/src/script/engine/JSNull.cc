#include "script/engine/JSNull.hpp"
#include "script/engine/JSNullType.hpp"
#include "script/util/JSSingleton.hpp"
JSNull::JSNull(JSAllocator *allocator)
    : JSBase(allocator, JSSingleton::instance<JSNullType>(allocator)) {}
