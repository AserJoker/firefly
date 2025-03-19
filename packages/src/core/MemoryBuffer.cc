#include "core/MemoryBuffer.hpp"
#include <cstring>
#include <new>
using namespace firefly;
using namespace firefly::core;
MemoryBuffer::MemoryBuffer(size_t size, const void *source)
    : _buf(nullptr), _size(size) {
  if (_size) {
    _buf = ::operator new(_size);
    if (source) {
      std::memcpy(_buf, source, _size);
    } else {
      std::memset(_buf, 0, _size);
    }
  }
}

MemoryBuffer::MemoryBuffer(const core::AutoPtr<Buffer> &buffer)
    : MemoryBuffer(buffer->getSize(), buffer->getRaw()) {}

MemoryBuffer::~MemoryBuffer() {
  if (_size) {
    ::operator delete(_buf);
    _buf = nullptr;
    _size = 0;
  }
}

const void *MemoryBuffer::getRaw() const { return _buf; }

size_t MemoryBuffer::getSize() const { return _size; }

void MemoryBuffer::resize(size_t size) {
  if (size != _size) {
    void *buf = ::operator new(size);
    std::memset(buf, 0, size);
    if (_buf) {
      std::memcpy(buf, _buf, std::min(_size, size));
      ::operator delete(_buf);
    }
    _buf = buf;
    _size = size;
  }
}

const core::AutoPtr<Buffer> MemoryBuffer::read(size_t offset,
                                               size_t size) const {
  if (offset >= _size) {
    return new MemoryBuffer();
  }
  if (size == 0) {
    return new MemoryBuffer(_size - offset, (uint8_t *)_buf + offset);
  }
  return new MemoryBuffer(std::min(_size - offset, size),
                          (uint8_t *)_buf + offset);
};

void MemoryBuffer::write(const core::AutoConstPtr<Buffer> &buffer,
                         size_t offset) {
  if (offset + buffer->getSize() > _size) {
    resize(offset + buffer->getSize());
  }
  std::memcpy((uint8_t *)_buf + offset, buffer->getRaw(), buffer->getSize());
}
bool MemoryBuffer::isReadable() const { return true; }
bool MemoryBuffer::isWritable() const { return true; }