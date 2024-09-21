#include "video/Attribute.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
#include <glm/glm.hpp>
using namespace firefly;
using namespace firefly::video;

Attribute::Attribute(const core::AutoPtr<core::Buffer> &buffer,
                     const std::type_info &type, uint32_t itemSize,
                     const bool &normalize, bool dynamic)
    : Attribute(buffer->getSize(), buffer->getData(), type, itemSize, normalize,
                dynamic) {}

Attribute::Attribute(uint32_t size, const void *buffer,
                     const std::type_info &type, uint32_t itemSize,
                     const bool &normalize, bool dynamic)
    : _itemType(type.name()), _itemSize(itemSize), _normalize(normalize),
      _dynamic(dynamic) {
  if (type == typeid(float)) {
    _stride = sizeof(float) * itemSize;
  } else if (type == typeid(int32_t)) {
    _stride = sizeof(int32_t) * itemSize;
  } else if (type == typeid(uint32_t)) {
    _stride = sizeof(uint32_t) * itemSize;
  } else if (type == typeid(double)) {
    _stride = sizeof(double) * itemSize;
  } else if (type == typeid(int8_t)) {
    _stride = sizeof(int8_t) * itemSize;
  } else if (type == typeid(uint8_t)) {
    _stride = sizeof(uint8_t) * itemSize;
  } else if (type == typeid(int16_t)) {
    _stride = sizeof(int16_t) * itemSize;
  } else if (type == typeid(uint16_t)) {
    _stride = sizeof(uint16_t) * itemSize;
  } else {
    throw std::runtime_error(
        fmt::format("Failed to init attribute with type '{}'", _itemType));
  }
  _vbo = new gl::Buffer(dynamic ? gl::BUFFER_USAGE::DYNAMIC_DRAW
                                : gl::BUFFER_USAGE::STATIC_DRAW);
  _vbo->setData(size, buffer);
  _itemCount = size / _stride;
}

const std::string &Attribute::getItemType() const { return _itemType; }

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