#include "runtime/Resource_Buffer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include <cstring>
using namespace firefly;
using namespace firefly::runtime;

Resource_Buffer::Resource_Buffer(const void *buffer, const uint32_t &size)
    : _buffer(nullptr), _size(size), _pos(0) {
  if (size) {
    _buffer = ::operator new(size);
    std::memcpy(_buffer, buffer, size);
  }
}
Resource_Buffer::~Resource_Buffer() {
  if (_buffer) {
    ::operator delete(_buffer);
  }
};
core::AutoPtr<core::Buffer> Resource_Buffer::read(const size_t &size) {
  if (_pos == _size) {
    return new core::Buffer(0);
  }
  if (size == 0) {
    auto len = _size - _pos;
    return new core::Buffer(_size - _pos, (char *)_buffer + _pos);
  }
  return new core::Buffer(size > _size - _pos ? _size - _pos : size,
                          (char *)_buffer + _pos);
};
void Resource_Buffer::write(const core::AutoPtr<core::Buffer> &buffer) {
  auto &size = buffer->getSize();
  auto buf = buffer->getData();
  if (_pos + size > _size) {
    void *buffer = ::operator new(_pos + size);
    memcpy(buffer, _buffer, _pos);
    ::operator delete(_buffer);
    _size = _pos + size;
  }
  memcpy((char *)_buffer + _pos, buf, size);
  _pos += size;
};