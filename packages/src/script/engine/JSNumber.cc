#include "script/engine/JSNumber.hpp"
#include "script/engine/JSNumberType.hpp"
#include "script/util/JSSingleton.hpp"

JSNumber::JSNumber(JSAllocator *allocator, double value)
    : JSBase(allocator, JSSingleton::instance<JSNumberType>(allocator)),
      _value(value) {}
