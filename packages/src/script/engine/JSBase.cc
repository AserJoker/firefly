#include "script/engine/JSBase.hpp"
JSBase::JSBase(JSAllocator *allocator, const JS_TYPE &type)
    : _allocator(allocator), _ref(0), _type(type) {}

void JSBase::release() {
  if (!dispose()) {
    _allocator->dispose(this);
  }
}