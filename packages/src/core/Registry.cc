#include "core/Registry.hpp"
using namespace firefly;
using namespace firefly::core;
void Registry::store(const std::string &name,
                     const core::AutoPtr<ObjectBase> &object) {
  if (!object) {
    _registrys.erase(name);
  } else {
    _registrys[name] = object;
  }
}

core::AutoPtr<ObjectBase> Registry::query(const std::string &name) {
  if (_registrys.contains(name)) {
    return _registrys.at(name);
  }
  return nullptr;
}

bool Registry::contains(const std::string &name) {
  return _registrys.contains(name);
}