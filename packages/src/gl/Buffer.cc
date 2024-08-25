#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
using namespace firefly;
using namespace firefly::gl;
Buffer::Buffer(BUFFER_USAGE usage, const uint32_t &handle, const uint32_t &size)
    : _usage(usage), _handle(handle), _size(size) {
  if (!_handle) {
    glGenBuffers(1, &_handle);
    if (size) {
      glBindBuffer(GL_ARRAY_BUFFER, _handle);
      glBufferData(GL_ARRAY_BUFFER, size, 0, (GLenum)usage);
    }
  }
}

void Buffer::write(const uint32_t &size, void *data) {
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  glBufferData(GL_ARRAY_BUFFER, size, data, (GLenum)_usage);
}
void Buffer::write(const uint32_t &offset, const uint32_t &size, void *data) {
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}
Buffer::~Buffer() {
  if (_handle) {
    glDeleteBuffers(1, &_handle);
    _handle = 0;
  }
}
void Buffer::bind(BUFFER_TARGET target, const core::AutoPtr<Buffer> &buffer) {
  glBindBuffer((GLenum)target, buffer->_handle);
}