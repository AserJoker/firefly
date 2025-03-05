#include "script/engine/JSNativeFunctionType.hpp"
JSNativeFunctionType::JSNativeFunctionType(JSAllocator *allocator)
    : JSCallableType(allocator) {}