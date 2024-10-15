#include "gl/Buffer.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/BufferUsage.hpp"
using namespace firefly;
using namespace firefly::gl;
Buffer::Buffer(BUFFER_USAGE usage, uint32_t handle)
    : _handle(handle), _usage(usage) {
  if (!_handle) {
    glGenBuffers(1, &_handle);
  }
}

void Buffer::setData(uint32_t size, const void *data) {
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  glBufferData(GL_ARRAY_BUFFER, size, data, (GLenum)_usage);
}
void Buffer::write(uint32_t offset, uint32_t size, const void *data) {
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}
const void *Buffer::map(MAP_ACCESS access) const {
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  return glMapBuffer(GL_ARRAY_BUFFER, (GLenum)access);
}
void *Buffer::map(MAP_ACCESS access) {
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  return glMapBuffer(GL_ARRAY_BUFFER, (GLenum)access);
}
void Buffer::unmap() const {
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  glUnmapBuffer(GL_ARRAY_BUFFER);
}
const uint32_t Buffer::getBufferSize() const {
  int32_t size;
  glBindBuffer(GL_ARRAY_BUFFER, _handle);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  return size;
}
Buffer::~Buffer() {
  if (_handle) {
    glDeleteBuffers(1, &_handle);
    _handle = 0;
  }
}
void Buffer::bind(BUFFER_TARGET target, const core::AutoPtr<Buffer> &buffer) {
  if (!buffer) {
    glBindBuffer((GLenum)target, 0);
  } else {
    glBindBuffer((GLenum)target, buffer->_handle);
  }
}
void Buffer::bindBase(BUFFER_TARGET target, uint32_t index,
                      const core::AutoPtr<Buffer> &buffer) {
  glBindBufferBase((GLenum)target, index, buffer->_handle);
}