#include "video/Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
using namespace firefly;
using namespace firefly::video;

Geometry::Geometry() { _indices = new AttributeIndex(); }
void Geometry::setAttribute(const uint32_t &index,
                            const core::AutoPtr<Attribute> &attribute) {
  if (_attributes.contains(index) && _attributes.at(index) == attribute) {
    return;
  }
  _attributes[index] = attribute;
  setVersion(getVersion() + 1);
}
core::AutoPtr<Attribute> Geometry::getAttribute(const uint32_t &index) {
  if (!_attributes.contains(index)) {
    return nullptr;
  }
  return _attributes.at(index);
}
const core::AutoPtr<Attribute>
Geometry::getAttribute(const uint32_t &index) const {
  if (!_attributes.contains(index)) {
    return nullptr;
  }
  return _attributes.at(index);
}
void Geometry::removeAttribute(const uint32_t &index) {
  _attributes.erase(index);
  setVersion(getVersion() + 1);
}
const std::unordered_map<uint32_t, core::AutoPtr<Attribute>> &
Geometry::getAttributes() const {
  return _attributes;
}
const core::AutoPtr<AttributeIndex> &Geometry::getIndices() const {
  return _indices;
}
core::AutoPtr<AttributeIndex> &Geometry::getIndices() { return _indices; }