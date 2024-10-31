#include "core/Buffer.hpp"
#include <cstring>
using namespace firefly;
using namespace firefly::core;
Buffer::Buffer(core::Unsigned_t size, const void *data)
    : _size(size), _data(0) {
  if (size) {
    _data = ::operator new(size);
    if (data) {
      std::memcpy(_data, data, size);
    } else {
      memset(_data, 0, size);
    }
  }
}
Buffer::~Buffer() {
  if (_data) {
    ::operator delete(_data);
  }
}
const void *Buffer::getData() const { return _data; }
const core::Unsigned_t &Buffer::getSize() const { return _size; }
void Buffer::setData(core::Unsigned_t offset, core::Unsigned_t size,
                     const void *data) {
  if (offset + size > _size) {
    void *buf = ::operator new(offset + size);
    std::memcpy(buf, _data, _size);
    _size = offset + size;
    ::operator delete(_data);
    _data = buf;
  }
  std::memcpy((char *)_data + offset, data, size);
}