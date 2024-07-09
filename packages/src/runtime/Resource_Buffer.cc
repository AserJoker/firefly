#include "runtime/Resource_Buffer.hpp"
#include <cstring>
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
void *Resource_Buffer::read(const size_t &size, size_t *len) {
  if (_pos == _size) {
    *len = 0;
    return nullptr;
  }
  if (size == 0) {
    void *buf = ::operator new(_size - _pos);
    memcpy(buf, (char *)_buffer + _pos, _size - _pos);
    *len = _size - _pos;
    _pos = _size;
    return buf;
  }
  void *buf = ::operator new(size);
  *len = size > _size - _pos ? _size - _pos : size;
  memcpy(buf, (char *)_buffer + _pos, *len);
  _pos += *len;
  return buf;
};
void Resource_Buffer::write(void *buf, const size_t &size) {
  if (_pos + size > _size) {
    void *buffer = ::operator new(_pos + size);
    memcpy(buffer, _buffer, _pos);
    ::operator delete(_buffer);
    _size = _pos + size;
  }
  memcpy((char *)_buffer + _pos, buf, size);
  _pos += size;
};