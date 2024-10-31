#include "video/Attribute.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
#include <glm/glm.hpp>
using namespace firefly;
using namespace firefly::video;

Attribute::Attribute(const core::AutoPtr<core::Buffer> &buffer,
                     core::Unsigned_t itemSize,
                     const core::Boolean_t &normalize, core::Boolean_t dynamic)
    : Attribute(buffer->getSize(), buffer->getData(), itemSize, normalize,
                dynamic) {}

Attribute::Attribute(core::Unsigned_t size, const void *buffer,
                     core::Unsigned_t itemSize,
                     const core::Boolean_t &normalize, core::Boolean_t dynamic)
    : _itemSize(itemSize), _normalize(normalize), _dynamic(dynamic) {
  _stride = sizeof(core::Float_t) * itemSize;
  _vbo = new gl::Buffer(dynamic ? gl::BUFFER_USAGE::DYNAMIC_DRAW
                                : gl::BUFFER_USAGE::STATIC_DRAW);
  _vbo->setData(size, buffer);
  _itemCount = size / _stride;
}

const core::Unsigned_t &Attribute::getItemSize() const { return _itemSize; }

const core::Boolean_t &Attribute::isNormalized() const { return _normalize; }

const core::Boolean_t &Attribute::isDynamic() const { return _dynamic; }

const core::AutoPtr<gl::Buffer> &Attribute::getVertexBufferObject() const {
  return _vbo;
}
const core::Unsigned_t &Attribute::getStride() const { return _stride; }

const core::Unsigned_t &Attribute::getItemCount() const { return _itemCount; }
void Attribute::write(core::Unsigned_t offset, core::Unsigned_t size,
                      const void *data) {
  _vbo->write(offset, size, data);
}
void Attribute::write(core::Unsigned_t size, const void *data) {
  _vbo->setData(size, data);
}