#include "video/VertexArray.hpp"
#include "video/Buffer.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::video;
void VertexArray::bind() const { glBindVertexArray(_handle); }
void VertexArray::unbind() const { glBindVertexArray(0); }
VertexArray::VertexArray(const VertexAttributeTable &attrs,
                         const std::vector<core::AutoPtr<Buffer>> &buffers)
    : _handle(0) {
  static uint32_t sizeMap[] = {sizeof(char),  sizeof(char), sizeof(short),
                               sizeof(short), sizeof(int),  sizeof(uint32_t),
                               sizeof(float)};
  glGenVertexArrays(1, &_handle);
  if (!_handle) {
    throw std::runtime_error(
        fmt::format("Failed to create vertex array: 0x(:x)", glGetError()));
  }
  bind();
  for (auto &buf : buffers) {
    _buffers[buf->getType()] = buf;
    buf->bind();
  }
  size_t stride = 0;
  for (auto &attr : attrs) {
    stride += attr.size * sizeMap[(uint32_t)attr.type];
  }
  size_t offset = 0;
  for (int i = 0; i < attrs.size(); i++) {
    size_t step = sizeMap[(uint32_t)attrs[i].type];
    glVertexAttribPointer(i, attrs[i].size, (uint32_t)attrs[i].type + GL_BYTE,
                          GL_FALSE, stride, (void *)offset);
    glEnableVertexAttribArray(i);
    offset += step * attrs[i].size;
  }
  unbind();
}
VertexArray::~VertexArray() { glDeleteVertexArrays(1, &_handle); }
const VertexAttributeTable &VertexArray::getVertexAttributeTable() const {
  return _attributes;
}
const core::AutoPtr<Buffer>
VertexArray::getBuffer(const BufferType &type) const {
  if (!_buffers.contains(type)) {
    return nullptr;
  }
  return _buffers.at(type);
}
core::AutoPtr<Buffer> VertexArray::getBuffer(const BufferType &type) {
  if (!_buffers.contains(type)) {
    return nullptr;
  }
  return _buffers.at(type);
}