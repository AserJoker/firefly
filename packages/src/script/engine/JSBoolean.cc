#include "script/engine/JSBoolean.hpp"
#include "script/engine/JSString.hpp"
#include "script/util/JSAllocator.hpp"

JSBoolean::JSBoolean(JSAllocator *allocator, bool value)
    : JSBase(allocator, JS_TYPE::BOOLEAN), _value(value) {}
JSBase *JSBoolean::toString() {
  return getAllocator()->create<JSString>(_value ? L"true" : L"false");
}

JSBase *JSBoolean::clone() { return getAllocator()->create<JSBoolean>(_value); }