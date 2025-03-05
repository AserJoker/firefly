#include "script/engine/JSNaN.hpp"
#include "script/engine/JSBase.hpp"
#include "script/engine/JSNaNType.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"
JSNaN::JSNaN(JSAllocator *allocator)
    : JSBase(allocator, JSSingleton::instance<JSNaNType>(allocator)) {}