#include "video/Buffer.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::video;
Buffer::Buffer(const uint32_t &size, void *data, const BufferType &type,
               const BufferUsage &usage)
    : _type(type), _usage(usage), _size(size), _handle(0) {
  glGenBuffers(1, &_handle);
  if (!_handle) {
    throw std::runtime_error(
        fmt::format("Failed to create buffer: 0x{:x}", glGetError()));
  }
  bind();
  glBufferData((uint32_t)_type, size, data, (uint32_t)usage);
  unbind();
}

Buffer::~Buffer() { glDeleteBuffers(1, &_handle); }

void Buffer::bind() const { glBindBuffer((uint32_t)_type, _handle); }

void Buffer::unbind() const { glBindBuffer((uint32_t)_type, 0); }

const BufferType &Buffer::getType() const { return _type; }
const BufferUsage &Buffer::getUsage() const { return _usage; }
const uint32_t &Buffer::getSize() const { return _size; }