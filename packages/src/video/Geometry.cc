#include "video/Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "video/AttributeIndex.hpp"
using namespace firefly;
using namespace firefly::video;
Geometry::Geometry() : _needUpdate(true) { _indices = new AttributeIndex(); }
const core::AutoPtr<AttributeIndex> &Geometry::getIndices() const {
  return _indices;
}
core::AutoPtr<AttributeIndex> &Geometry::getIndices() { return _indices; }

const GeometryAttributes &Geometry::getAttributes() const {
  return _attributes;
}
GeometryAttributes &Geometry::getAttributes() { return _attributes; }

const core::AutoPtr<Attribute>
Geometry::getAttribute(const std::string &name) const {
  if (_attributes.contains(name)) {
    return _attributes.at(name);
  }
  return nullptr;
}

core::AutoPtr<Attribute> Geometry::getAttribute(const std::string &name) {
  if (_attributes.contains(name)) {
    return _attributes.at(name);
  }
  return nullptr;
}

void Geometry::setAttribute(const std::string &name,
                            const core::AutoPtr<Attribute> &attribute) {
  _attributes[name] = attribute;
  _needUpdate = true;
}

const bool Geometry::hasAttribute(const std::string &name) const {
  return _attributes.contains(name);
}

void Geometry::removeAttribute(const std::string &name) {
  _attributes.erase(name);
  _needUpdate = true;
}

core::AutoPtr<Geometry> Geometry::clone() {
  core::AutoPtr geo = new Geometry();
  for (auto &[name, attr] : _attributes) {
    geo->_attributes[name] = attr->clone();
  }
  geo->_indices = _indices->clone();
  return geo;
}

const std::vector<GeometryRenderRange> &Geometry::getRenderRanges() const {
  return _renderRanges;
}

void Geometry::setRenderRange(const uint32_t &start, const uint32_t &count) {
  _renderRanges.push_back({start, count});
}

const bool Geometry::isNeedUpdate() const { return _needUpdate; }
void Geometry::setNeedUpdate(const bool &value) { _needUpdate = value; }