#include "render/Attribute.hpp"
#include "core/Buffer.hpp"
#include "core/MemoryBuffer.hpp"
using namespace firefly;
using namespace firefly::render;
Attribute::Attribute(uint32_t itemCount,
                     const core::AutoPtr<core::Buffer> &buffer,
                     bool normalize) {
  _itemCount = itemCount;
  _normalize = normalize;
  _buffer = buffer;
  if (!_buffer) {
    _buffer = new core::MemoryBuffer();
  }
}

Attribute::Attribute(uint32_t itemCount, void *ptr, uint32_t size,
                     bool normalize) {
  _itemCount = itemCount;
  _buffer = new core::MemoryBuffer(size, ptr);
  _normalize = normalize;
}

bool Attribute::isNormalize() const { return _normalize; }

void Attribute::setIsNormalize(bool value) { _normalize = value; }

uint32_t Attribute::getItemCount() const { return _itemCount; }

void Attribute::setItemCount(uint32_t count) { _itemCount = count; }

const core::AutoPtr<core::Buffer> &Attribute::getBuffer() const {
  return _buffer;
}
void Attribute::setBuffer(const core::AutoPtr<core::Buffer> &buffer) {
  _buffer = buffer;
}