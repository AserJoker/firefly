#include "render/Geometory.hpp"
using namespace firefly;
using namespace firefly::render;
void Geometory::setAttribute(const std::string &name,
                             const core::AutoPtr<Attribute> &attribute) {
  _attributes[name] = attribute;
}

const core::AutoPtr<Attribute> &
Geometory::getAttribute(const std::string &name) const {
  return _attributes.at(name);
}

bool Geometory::hasAttribute(const std::string &name) const {
  return _attributes.contains(name);
}