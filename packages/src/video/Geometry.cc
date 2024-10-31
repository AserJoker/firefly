#include "video/Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "core/Map.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/DrawMode.hpp"
#include "gl/VertexArray.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"

using namespace firefly;
using namespace firefly::video;

Geometry::Geometry() { _vao = new gl::VertexArray(); }
Geometry::Geometry(const core::Array<core::AutoPtr<Attribute>> &attributes,
                   const core::AutoPtr<AttributeIndex> &index)
    : Geometry() {
  for (size_t i = 0; i < attributes.size(); i++) {
    setAttribute(i, attributes[i]);
  }
  setAttributeIndex(index);
}
void Geometry::setAttribute(core::Unsigned_t index,
                            const core::AutoPtr<Attribute> &attribute) {
  if (_attributes.contains(index) && _attributes.at(index) == attribute) {
    return;
  }
  _attributes[index] = attribute;

  gl::VertexArray::bind(_vao);
  gl::Buffer::bind(gl::BUFFER_TARGET::ARRAY,
                   attribute->getVertexBufferObject());
  gl::DATA_TYPE dtype = gl::DATA_TYPE::FLOAT;
  if (attribute->getItemSize() <= 4) {
    _vao->setAttribute(index, dtype, attribute->getItemSize(),
                       attribute->isNormalized(), attribute->getStride(), 0);
    _vao->enableAttribute(index);
  } else {
    core::Unsigned_t offset = 0;
    auto idx = index;
    auto valueSize = attribute->getStride() / attribute->getItemSize();
    while (offset < attribute->getItemSize()) {
      core::Unsigned_t itemSize = 4;
      if (offset + itemSize > attribute->getItemSize()) {
        itemSize = valueSize - itemSize;
      }
      _vao->setAttribute(idx, dtype, itemSize, attribute->isNormalized(),
                         attribute->getStride(), offset * valueSize);
      _vao->enableAttribute(idx);
      idx++;
      offset += itemSize;
    }
  }
}
core::AutoPtr<Attribute> Geometry::getAttribute(core::Unsigned_t index) {
  if (!_attributes.contains(index)) {
    return nullptr;
  }
  return _attributes.at(index);
}

const core::AutoPtr<Attribute>
Geometry::getAttribute(core::Unsigned_t index) const {
  if (!_attributes.contains(index)) {
    return nullptr;
  }
  return _attributes.at(index);
}

void Geometry::setVertexAttribDivisor(core::Unsigned_t index,
                                      core::Unsigned_t divisor) {
  gl::VertexArray::bind(_vao);
  _vao->setVertexAttribDivisor(index, divisor);
}

void Geometry::removeAttribute(core::Unsigned_t index) {
  _attributes.erase(index);
  _vao->disableAttribute(index);
}

const core::Map<core::Unsigned_t, core::AutoPtr<Attribute>> &
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

void Geometry::drawInstanced(gl::DRAW_MODE mode, core::Unsigned_t count) const {
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