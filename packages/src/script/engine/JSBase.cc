#include "script/engine/JSBase.hpp"
#include "script/engine/JSType.hpp"
JSBase::JSBase(JSAllocator *allocator, JSType *type)
    : JSRef(allocator), _type(type) {
  _type->addRef();
}

JSBase::~JSBase() {
  if (_type) {
    _type->release();
    _type = nullptr;
  }
}