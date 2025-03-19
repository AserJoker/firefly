#include "core/ObjectBase.hpp"
#include "core/Ref.hpp"
using namespace firefly;
using namespace firefly::core;
std::unordered_map<HANDLE, ObjectBase *> ObjectBase::_table;

ObjectBase *ObjectBase::select(const HANDLE &handle) {
  if (_table.contains(handle)) {
    return _table.at(handle);
  }
  return nullptr;
}

ObjectBase::ObjectBase() {
  static HANDLE _generator = 0;
  _handle = ++_generator;
  _table[_handle] = this;
}

ObjectBase::~ObjectBase() { _table.erase(_handle); }

const uint32_t &ObjectBase::ref() const { return Ref::ref(this); }

void *ObjectBase::operator new(size_t size) { return ::operator new(size); }

void ObjectBase::operator delete(void *opt) { ::operator delete(opt); }

const HANDLE &ObjectBase::getHandle() const { return _handle; }