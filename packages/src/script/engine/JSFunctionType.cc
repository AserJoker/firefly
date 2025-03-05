
#include "script/engine/JSFunctionType.hpp"
#include "script/engine/JSCallableType.hpp"
JSFunctionType::JSFunctionType(JSAllocator *allocator)
    : JSCallableType(allocator) {}