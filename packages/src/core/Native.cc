#include "core/Native.hpp"
#include "core/AutoPtr.hpp"
#include "core/ObjectBase.hpp"
using namespace firefly;
using namespace firefly::core;
void Native::setNative(const core::AutoPtr<ObjectBase> &native) {
  _native = native;
}

core::AutoPtr<ObjectBase> Native::getNative() { return _native; }

const core::AutoPtr<ObjectBase> &Native::getNative() const { return _native; }