#include "script/engine/JSUndefined.hpp"
#include "script/engine/JSBase.hpp"
#include "script/engine/JSUndefinedType.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"
JSUndefined::JSUndefined(JSAllocator *allocator)
    : JSBase(allocator, JSSingleton::instance<JSUndefinedType>(allocator)) {}