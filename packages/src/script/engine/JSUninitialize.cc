#include "script/engine/JSUninitialize.hpp"
#include "script/engine/JSBase.hpp"
#include "script/engine/JSUninitializeType.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"

JSUninitialize::JSUninitialize(JSAllocator *allocator)
    : JSBase(allocator, JSSingleton::instance<JSUninitializeType>(allocator)) {}