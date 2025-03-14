#include "script/engine/JSBigInt.hpp"
#include "script/engine/JSBase.hpp"
#include "script/engine/JSBigIntType.hpp"
#include "script/util/BigInt.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"

JSBigInt::JSBigInt(JSAllocator *allocator, const BigInt<> &value)
    : JSBase(allocator, JSSingleton::instance<JSBigIntType>(allocator)),
      _value(value) {}