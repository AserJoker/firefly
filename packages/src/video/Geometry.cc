#include "video/Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/DrawMode.hpp"
#include "gl/MapAccess.hpp"
#include "gl/VertexArray.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include <unordered_map>
using namespace firefly;
using namespace firefly::video;

Geometry::Geometry() : _binding({0, 0, 0, 0, 0, 0}) {
  _vao = new gl::VertexArray();
}
Geometry::Geometry(const std::vector<core::AutoPtr<Attribute>> &attributes,
                   const core::AutoPtr<AttributeIndex> &index)
    : Geometry() {
  for (size_t i = 0; i < attributes.size(); i++) {
    setAttribute(i, attributes[i]);
  }
  setAttributeIndex(index);
}
void Geometry::setAttribute(uint32_t index,
                            const core::AutoPtr<Attribute> &attribute) {
  if (_attributes.contains(index) && _attributes.at(index) == attribute) {
    return;
  }
  _attributes[index] = attribute;

  gl::VertexArray::bind(_vao);
  gl::Buffer::bind(gl::BUFFER_TARGET::ARRAY,
                   attribute->getVertexBufferObject());
  gl::DATA_TYPE dtype;
  auto &type = attribute->getItemType();
  if (type == typeid(float).name()) {
    dtype = gl::DATA_TYPE::FLOAT;
  } else if (type == typeid(int32_t).name()) {
    dtype = gl::DATA_TYPE::INT;
  } else if (type == typeid(uint32_t).name()) {
    dtype = gl::DATA_TYPE::UNSIGNED_INT;
  } else if (type == typeid(double).name()) {
    dtype = gl::DATA_TYPE::DOUBLE;
  } else if (type == typeid(int8_t).name()) {
    dtype = gl::DATA_TYPE::BTYE;
  } else if (type == typeid(uint8_t).name()) {
    dtype = gl::DATA_TYPE::UNSIGNED_BYTE;
  } else if (type == typeid(int16_t).name()) {
    dtype = gl::DATA_TYPE::SHORT;
  } else if (type == typeid(uint16_t).name()) {
    dtype = gl::DATA_TYPE::UNSIGNED_SHORT;
  }
  _vao->setAttribute(index, dtype, attribute->getItemSize(),
                     attribute->isNormalized(), attribute->getStride(), 0);
  _vao->enableAttribute(index);
}
core::AutoPtr<Attribute> Geometry::getAttribute(uint32_t index) {
  if (!_attributes.contains(index)) {
    return nullptr;
  }
  return _attributes.at(index);
}
const core::AutoPtr<Attribute> Geometry::getAttribute(uint32_t index) const {
  if (!_attributes.contains(index)) {
    return nullptr;
  }
  return _attributes.at(index);
}
void Geometry::removeAttribute(uint32_t index) {
  _attributes.erase(index);
  _vao->disableAttribute(index);
}
const std::unordered_map<uint32_t, core::AutoPtr<Attribute>> &
Geometry::getAttributes() const {
  return _attributes;
}
void Geometry::setAttributeIndex(
    const core::AutoPtr<AttributeIndex> &attrIndex) {
  _attrIndex = attrIndex;
  gl::VertexArray::bind(_vao);
  gl::Buffer::bind(gl::BUFFER_TARGET::ELEMENT_ARRAY,
                   attrIndex->getElementBufferObject());
}
const core::AutoPtr<AttributeIndex> &Geometry::getAttributeIndex() const {
  return _attrIndex;
}
core::AutoPtr<AttributeIndex> &Geometry::getAttributeIndex() {
  return _attrIndex;
}
const Geometry::Bounding &Geometry::getBounding() const { return _binding; }
void Geometry::computeBounding() {
  if (_attributes.contains(Geometry::ATTR_POSITION)) {
    auto &position = _attributes.at(Geometry::ATTR_POSITION);
    auto &itemSize = position->getItemSize();
    if (itemSize == 3) {
      glm::vec3 *data = (glm::vec3 *)position->getVertexBufferObject()->map(
          gl::MAP_ACCESS::READ_ONLY);
      auto count = position->getItemCount();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        _binding.left = std::min(_binding.left, item.x);
        _binding.right = std::max(_binding.right, item.x);
        _binding.bottom = std::min(_binding.bottom, item.y);
        _binding.top = std::max(_binding.top, item.y);
        _binding.front = std::min(_binding.front, item.z);
        _binding.back = std::max(_binding.back, item.z);
      }
      position->getVertexBufferObject()->unmap();
    } else if (itemSize == 2) {
      glm::vec2 *data = (glm::vec2 *)position->getVertexBufferObject()->map(
          gl::MAP_ACCESS::READ_ONLY);
      auto count = position->getItemCount();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        _binding.left = std::min(_binding.left, item.x);
        _binding.right = std::max(_binding.right, item.x);
        _binding.bottom = std::min(_binding.bottom, item.y);
        _binding.top = std::max(_binding.top, item.y);
      }
      position->getVertexBufferObject()->unmap();
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
      glm::vec3 *data = (glm::vec3 *)position->getVertexBufferObject()->map(
          gl::MAP_ACCESS::READ_ONLY);
      auto count = position->getItemCount();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        auto dst = shpere.center - item;
        auto d = std::sqrt(dst.x * dst.x + dst.y * dst.y + dst.z * dst.z);
        if (d > shpere.radis) {
          shpere.radis = d;
        }
      }
      position->getVertexBufferObject()->unmap();
    } else {
      glm::vec2 *data = (glm::vec2 *)position->getVertexBufferObject()->map(
          gl::MAP_ACCESS::READ_ONLY);
      auto count = position->getItemCount();
      for (uint32_t i = 0; i < count; i++) {
        auto &item = data[i];
        auto dst = glm::vec2(shpere.center) - item;
        auto d = std::sqrt(dst.x * dst.x + dst.y * dst.y);
        if (d > shpere.radis) {
          shpere.radis = d;
        }
      }
      position->getVertexBufferObject()->unmap();
    }
  }
  _bindingShpere = shpere;
}
const Geometry::BoundingShpere &Geometry::getBoundingSphere() const {
  return _bindingShpere;
}
void Geometry::draw(gl::DRAW_MODE mode) const {
  if (!_attributes.contains(Geometry::ATTR_POSITION)) {
    return;
  }
  gl::VertexArray::bind(_vao);
  if (!_attrIndex) {
    glDrawArrays((GLenum)mode, 0,
                 _attributes.at(Geometry::ATTR_POSITION)->getItemCount());
  } else {
    glDrawElements((GLenum)mode, _attrIndex->getIndicesCount(), GL_UNSIGNED_INT,
                   0);
  }
}
void Geometry::drawInstanced(gl::DRAW_MODE mode, uint32_t count) const {
  if (!_attributes.contains(Geometry::ATTR_POSITION)) {
    return;
  }
  gl::VertexArray::bind(_vao);
  if (!_attrIndex) {
    glDrawArraysInstanced(
        (GLenum)mode, 0,
        _attributes.at(Geometry::ATTR_POSITION)->getItemCount(), count);
  } else {
    glDrawElementsInstanced((GLenum)mode, _attrIndex->getIndicesCount(),
                            GL_UNSIGNED_INT, 0, count);
  }
}