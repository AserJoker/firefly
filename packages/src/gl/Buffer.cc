#include "gl/Buffer.hpp"
#include "exception/OpenGLVersionException.hpp"
#include "gl/BufferTraget.hpp"
#include "gl/BufferUsage.hpp"
#include "gl/Device.hpp"
#include "gl/OpenGLException.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
using exception::OpenGLVersionException;
Buffer::Buffer(uint32_t handle) : _handle(handle), _autoDelete(false) {
  if (_handle == 0) {
    glGenBuffers(1, &_handle);
    _autoDelete = true;
  }
}
Buffer::~Buffer() {
  if (_autoDelete) {
    glDeleteBuffers(1, &_handle);
  }
}

void Buffer::setData(size_t size, void *data, BUFFER_USAGE usage) {
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glBufferData(GL_ARRAY_BUFFER, size, data, (GLenum)usage);
  CHECK_OPENGL("Failed to create buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}
void Buffer::storageData(size_t size, void *data, uint32_t flag) {
  CHECK_VERSION(4, 4, "Failed to create storage data");
  Device::checkVersion(4, 4);
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glBufferStorage(GL_ARRAY_BUFFER, size, data, flag);
  CHECK_OPENGL("Failed to create storage data");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}

void Buffer::write(int64_t offset, size_t size, void *data) {
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
  CHECK_OPENGL("Failed to write buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}
void Buffer::read(int64_t offset, size_t size, void *output) {
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferSubData(GL_ARRAY_BUFFER, offset, size, output);
  CHECK_OPENGL("Failed to read buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}

void Buffer::copy(core::AutoPtr<Buffer> target, int64_t srcOffset,
                  int64_t offset, size_t size) {
  CHECK_VERSION(3, 1, "Failed to copy buffer data");
  Buffer::bind(BUFFER_TARGET::COPY_READ, this);
  Buffer::bind(BUFFER_TARGET::COPY_WRITE, target);
  glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset,
                      offset, size);
  CHECK_OPENGL("Failed to copy buffer data");
  Buffer::unbind(BUFFER_TARGET::COPY_WRITE);
  Buffer::unbind(BUFFER_TARGET::COPY_READ);
}

void Buffer::map(size_t *size, void **output, ACCESS_TYPE access) {
  *size = getSize();
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  *output = glMapBuffer(GL_ARRAY_BUFFER, (GLenum)access);
  CHECK_OPENGL("Failed to map buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}
void Buffer::mapRange(size_t *size, void **output, int64_t offset,
                      size_t length, uint32_t access) {
  CHECK_VERSION(3, 0, "Failed to map range buffer");
  *size = getSize();
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  *output = glMapBufferRange(GL_ARRAY_BUFFER, offset, length, (GLenum)access);
  CHECK_OPENGL("Failed to map buffer range");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}

void Buffer::unmap() {
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glUnmapBuffer(GL_ARRAY_BUFFER);
  CHECK_OPENGL("Failed to unlock buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}

void Buffer::flush(int64_t offset, size_t length) {
  CHECK_VERSION(3, 0, "Failed to flush buffer");
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glFlushMappedBufferRange(GL_ARRAY_BUFFER, offset, length);
  CHECK_OPENGL("Failed to flush buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}

void Buffer::invalidate() {
  CHECK_VERSION(4, 3, "Failed to invalidate buffer");
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glInvalidateBufferData(GL_ARRAY_BUFFER);
  CHECK_OPENGL("Failed to invalidate buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}
void Buffer::invalidate(int64_t offset, size_t length) {
  CHECK_VERSION(4, 3, "Failed to invalidate buffer");
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glInvalidateBufferSubData(GL_ARRAY_BUFFER, offset, length);
  CHECK_OPENGL("Failed to invalidate buffer");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
}

ACCESS_TYPE Buffer::getAccessType() {
  GLint value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_ACCESS, &value);
  CHECK_OPENGL("Failed to get buffer access type");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return (ACCESS_TYPE)value;
}
uint32_t Buffer::getAccessFlags() {
  GLint value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_ACCESS_FLAGS, &value);
  CHECK_OPENGL("Failed to get buffer access flags");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return value;
}
bool Buffer::isImmutableStorage() {
  GLint value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_IMMUTABLE_STORAGE, &value);
  CHECK_OPENGL("Failed to check buffer is immutable storage");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return value;
}
bool Buffer::isMapped() {
  GLint value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAPPED, &value);
  CHECK_OPENGL("Failed to check buffer is mapped");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return value;
}
size_t Buffer::getMapLength() {
  GLint64 value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_MAP_LENGTH, &value);
  CHECK_OPENGL("Failed to get buffer map length");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return value;
}
int64_t Buffer::getMapOffset() {
  GLint64 value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_MAP_OFFSET, &value);
  CHECK_OPENGL("Failed to get buffer map offset");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return value;
}
size_t Buffer::getSize() {
  GLint64 value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &value);
  CHECK_OPENGL("Failed to get buffer size");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return value;
}
uint32_t Buffer::getStorageFlags() {
  GLint value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_STORAGE_FLAGS, &value);
  CHECK_OPENGL("Failed to get buffer storage flags");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return value;
}
BUFFER_USAGE Buffer::getUsage() {
  GLint value;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &value);
  CHECK_OPENGL("Failed to get buffer usage");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return (BUFFER_USAGE)value;
}

void *Buffer::getMappedPointer() {
  void *data = 0;
  Buffer::bind(BUFFER_TARGET::ARRAY, this);
  glGetBufferPointerv(GL_ARRAY_BUFFER, GL_BUFFER_MAP_POINTER, &data);
  CHECK_OPENGL("Failed to get buffer usage");
  Buffer::unbind(BUFFER_TARGET::ARRAY);
  return data;
}

uint32_t Buffer::getHandle() { return _handle; }

void Buffer::bind(BUFFER_TARGET target, core::AutoPtr<Buffer> buffer) {
  glBindBuffer((GLenum)target, buffer->_handle);
  CHECK_OPENGL("Failed to bind buffer");
}

void Buffer::unbind(BUFFER_TARGET target) {
  glBindBuffer((GLenum)target, 0);
  CHECK_OPENGL("Failed to bind buffer");
}

void Buffer::bindRange(BUFFER_TARGET target, uint32_t index,
                       core::AutoPtr<Buffer> buffer, uint32_t offset,
                       uint32_t size) {
  CHECK_VERSION(3, 0, "Failed to bind buffer range");
  glBindBufferRange((GLenum)target, index, buffer->_handle, offset, size);
  CHECK_OPENGL("Failed to bind buffer range");
}

void Buffer::bindBase(BUFFER_TARGET target, uint32_t index,
                      core::AutoPtr<Buffer> buffer) {
  CHECK_VERSION(3, 0, "Failed to bind buffer base");
  glBindBufferBase((GLenum)target, index, buffer->_handle);
  CHECK_OPENGL("Failed to bind buffer base");
}