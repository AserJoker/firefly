#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/util/JSAllocator.hpp"
JSCallableType::JSCallableType(JSAllocator *allocator)
    : JSObjectType(allocator) {}
const wchar_t *JSCallableType::getTypeName() const { return L"function"; }
