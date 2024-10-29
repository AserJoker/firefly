#include "video/Attribute.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
#include <glm/glm.hpp>
using namespace firefly;
using namespace firefly::video;

Attribute::Attribute(const core::AutoPtr<core::Buffer> &buffer,
                     uint32_t itemSize, const bool &normalize, bool dynamic)
    : Attribute(buffer->getSize(), buffer->getData(), itemSize, normalize,
                dynamic) {}

Attribute::Attribute(uint32_t size, const void *buffer, uint32_t itemSize,
                     const bool &normalize, bool dynamic)
    : _itemSize(itemSize), _normalize(normalize), _dynamic(dynamic) {
  _stride = sizeof(float) * itemSize;
  _vbo = new gl::Buffer(dynamic ? gl::BUFFER_USAGE::DYNAMIC_DRAW
                                : gl::BUFFER_USAGE::STATIC_DRAW);
  _vbo->setData(size, buffer);
  _itemCount = size / _stride;
}

const uint32_t &Attribute::getItemSize() const { return _itemSize; }

const bool &Attribute::isNormalized() const { return _normalize; }

const bool &Attribute::isDynamic() const { return _dynamic; }

const core::AutoPtr<gl::Buffer> &Attribute::getVertexBufferObject() const {
  return _vbo;
}
const uint32_t &Attribute::getStride() const { return _stride; }

const uint32_t &Attribute::getItemCount() const { return _itemCount; }
void Attribute::write(uint32_t offset, uint32_t size, const void *data) {
  _vbo->write(offset, size, data);
}
void Attribute::write(uint32_t size, const void *data) {
  _vbo->setData(size, data);
}