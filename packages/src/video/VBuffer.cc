#include "video/VBuffer.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::video;
VBuffer::VBuffer(const uint32_t &size, void *data, const VBufferType &type,
                 const VBufferUsage &usage)
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

VBuffer::~VBuffer() { glDeleteBuffers(1, &_handle); }

void VBuffer::bind() const { glBindBuffer((uint32_t)_type, _handle); }

void VBuffer::unbind() const { glBindBuffer((uint32_t)_type, 0); }

const VBufferType &VBuffer::getType() const { return _type; }
const VBufferUsage &VBuffer::getUsage() const { return _usage; }
const uint32_t &VBuffer::getSize() const { return _size; }
void VBuffer::lock(size_t *size, void **ppBuffer) {
  bind();
  *ppBuffer = glMapBuffer((uint32_t)_type, GL_READ_WRITE);
  unbind();
}
void VBuffer::unlock() {
  bind();
  glUnmapBuffer((uint32_t)_type);
  unbind();
}