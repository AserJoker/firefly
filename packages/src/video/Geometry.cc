#include "video/Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include <algorithm>
#include <unordered_map>
using namespace firefly;
using namespace firefly::video;

Geometry::Geometry() : _binding({0, 0, 0, 0, 0, 0}) {
  _indices = new AttributeIndex();
}
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
const Geometry::Bounding &Geometry::getBounding() const { return _binding; }
void Geometry::computeBounding() {
  if (_attributes.contains(Geometry::ATTR_POSITION)) {
    auto &position = _attributes.at(Geometry::ATTR_POSITION);
    auto &itemSize = position->getItemSize();
    if (itemSize == 3) {
      glm::vec3 *data = (glm::vec3 *)position->getBuffer()->getData();
      auto count = position->getBuffer()->getSize() / position->getStride();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        _binding.left = std::min(_binding.left, item.x);
        _binding.right = std::max(_binding.right, item.x);
        _binding.bottom = std::min(_binding.bottom, item.y);
        _binding.top = std::max(_binding.top, item.y);
        _binding.front = std::min(_binding.front, item.z);
        _binding.back = std::max(_binding.back, item.z);
      }
    } else if (itemSize == 2) {
      glm::vec2 *data = (glm::vec2 *)position->getBuffer()->getData();
      auto count = position->getBuffer()->getSize() / position->getStride();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        _binding.left = std::min(_binding.left, item.x);
        _binding.right = std::max(_binding.right, item.x);
        _binding.bottom = std::min(_binding.bottom, item.y);
        _binding.top = std::max(_binding.top, item.y);
      }
    }
  }
}
void Geometry::computeBoundingSphere() {
  auto y = (_binding.top + _binding.bottom) / 2;
  auto z = (_binding.front + _binding.back) / 2;
  auto x = (_binding.left + _binding.right) / 2;
  Geometry::BoundingShpere shpere = {glm::vec3(x, y, z), 0};
  if (_attributes.contains(Geometry::ATTR_POSITION)) {
    auto &position = _attributes.at(Geometry::ATTR_POSITION);
    auto &itemSize = position->getItemSize();
    if (itemSize == 3) {
      glm::vec3 *data = (glm::vec3 *)position->getBuffer()->getData();
      auto count = position->getBuffer()->getSize() / position->getStride();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        auto dst = shpere.center - item;
        auto d = std::sqrt(dst.x * dst.x + dst.y * dst.y + dst.z * dst.z);
        if (d > shpere.radis) {
          shpere.radis = d;
        }
      }
    } else {
      glm::vec2 *data = (glm::vec2 *)position->getBuffer()->getData();
      auto count = position->getBuffer()->getSize() / position->getStride();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        auto dst = glm::vec2(shpere.center) - item;
        auto d = std::sqrt(dst.x * dst.x + dst.y * dst.y);
        if (d > shpere.radis) {
          shpere.radis = d;
        }
      }
    }
  }
  _bindingShpere = shpere;
}
const Geometry::BoundingShpere &Geometry::getBoundingSphere() const {
  return _bindingShpere;
}