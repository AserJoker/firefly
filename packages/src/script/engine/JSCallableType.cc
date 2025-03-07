#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
#include <format>
JSCallableType::JSCallableType(JSAllocator *allocator)
    : JSObjectType(allocator) {}
const wchar_t *JSCallableType::getTypeName() const { return L"function"; }
