#include "script/engine/JSNull.hpp"
#include "script/engine/JSString.hpp"
JSNull::JSNull(JSAllocator *allocator) : JSBase(allocator, JS_TYPE::OBJECT) {}
JSBase *JSNull ::toString() {
  return getAllocator()->create<JSString>(L"null");
}

JSBase *JSNull ::clone() { return this; }