#include "core/Buffer.hpp"
#include <cstring>
using namespace firefly::core;
Buffer::Buffer(const uint32_t &size, void *data) : _size(size), _data(0) {
  if (size) {
    _data = ::operator new(size);
    if (data) {
      std::memcpy(_data, data, size);
    }
  }
}
Buffer::~Buffer() {
  if (_data) {
    ::operator delete(_data);
  }
}
const void *Buffer::getData() const { return _data; }
const uint32_t &Buffer::getSize() const { return _size; }
void Buffer::setData(const uint32_t &offset, const uint32_t &size, void *data) {
  if (offset + size > _size) {
    void *buf = ::operator new(offset + size);
    std::memcpy(buf, _data, _size);
    _size = offset + size;
    ::operator delete(_data);
    _data = buf;
  }
  std::memcpy((char *)_data + offset, data, size);
}