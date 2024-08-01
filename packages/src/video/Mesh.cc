#include "video/Mesh.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;

Mesh::Mesh(const std::vector<VertexFormat> &format) : _vf(format) {
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);

  auto index = 0;
  auto stride = 0;
  for (auto &fmt : _vf) {
    stride += fmt.stride * fmt.size;
  }
  ptrdiff_t offset = 0;
  for (auto &fmt : _vf) {
    glVertexAttribPointer(index, fmt.size, fmt.type, GL_FALSE, stride,
                          (void *)offset);
    glEnableVertexAttribArray(index);
    offset += fmt.size * fmt.stride;
    index++;
  }
  glBindVertexArray(0);
  _elements = 0;
}
Mesh::~Mesh() {
  glDeleteBuffers(1, &_ebo);
  glDeleteBuffers(1, &_vbo);
  glDeleteVertexArrays(1, &_vao);
}
void Mesh::draw() {
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, 0);
}
void Mesh::setVertexData(void *data, const uint32_t &size) {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Mesh::setElementData(uint32_t *data, const uint32_t &size) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  _elements = size / sizeof(uint32_t);
}
void Mesh::lockVertexBuffer(void **ppBuffer, uint32_t *nSize) {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, (int32_t *)nSize);
  *ppBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
}
void Mesh::unlockVertexBuffer() {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glUnmapBuffer(GL_ARRAY_BUFFER);
}
void Mesh::lockElementBuffer(void **ppBuffer, uint32_t *nSize) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo);
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE,
                         (int32_t *)nSize);
  *ppBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
}
void Mesh::unlockElementBuffer() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo);
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}