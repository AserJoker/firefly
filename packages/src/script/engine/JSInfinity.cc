#include "script/engine/JSInfinity.hpp"
#include "script/engine/JSBase.hpp"
#include "script/engine/JSInfinityType.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"
JSInfinity::JSInfinity(JSAllocator *allocator, bool negative)
    : JSBase(allocator, JSSingleton::instance<JSInfinityType>(allocator)),
      _negative(negative) {}

bool JSInfinity::isNegative() const { return _negative; }