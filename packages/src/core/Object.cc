#include "core/Object.hpp"
using namespace firefly;
using namespace firefly::core;
void Object::setMetadata(const std::string &name,
                         const core::AutoPtr<Object> &data) {
  _metadata[name] = data;
};
core::AutoPtr<Object> Object::getMetadata(const std::string &name) {
  if (_metadata.contains(name)) {
    return _metadata.at(name);
  }
  return nullptr;
};
const core::AutoPtr<Object> Object::getMetadata(const std::string &name) const {
  if (_metadata.contains(name)) {
    return _metadata.at(name);
  }
  return nullptr;
};
void Object::setVersion(const uint32_t &version) { _version = version; }
const uint32_t &Object::getVersion() const { return _version; }