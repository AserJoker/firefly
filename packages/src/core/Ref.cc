#include "core/Ref.hpp"
using namespace firefly;
using namespace firefly::core;
uint32_t Ref::addRef(const void *instance) {
  if (!_refs.contains(instance)) {
    _refs[instance] = 0;
  }
  return ++_refs.at(instance);
}

uint32_t Ref::dispose(const void *instance) {
  if (!_refs.contains(instance)) {
    return 0;
  }
  auto ref = --_refs.at(instance);
  if (!ref) {
    _refs.erase(instance);
    return 0;
  }
  return ref;
}

const uint32_t &Ref::ref(const void *instance) {
  static uint32_t zero = 0;
  if (_refs.contains(instance)) {
    return _refs.at(instance);
  }
  return zero;
}