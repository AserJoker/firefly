#include "core/RefBuffer.hpp"
using namespace firefly;
using namespace firefly::core;

RefBuffer::RefBuffer(const core::AutoConstPtr<Buffer> &source, size_t offset,
                     size_t size)
    : _source(source), _offset(offset), _size(size) {}
const void *RefBuffer::getRaw() const {
  return (uint8_t *)(_source->getRaw()) + _offset;
}

size_t RefBuffer::getSize() const {
  if (_offset >= _source->getSize()) {
    return 0;
  }
  return std::min(_source->getSize() - _offset, _size);
}