#include "gl/VertexArray.hpp"
using namespace firefly;
using namespace firefly::gl;

VertexArray::VertexArray(const uint32_t &handle) : _handle(handle) {
  if (!_handle) {
    glGenVertexArrays(1, &_handle);
  }
}
VertexArray::~VertexArray() {
  if (_handle) {
    glDeleteVertexArrays(1, &_handle);
    _handle = 0;
  }
}
void VertexArray::setAttribute(const uint32_t &index, DATA_TYPE dtype,
                               const uint32_t &size, const bool &normalized,
                               const uint32_t &stride, const uint32_t &offset) {
  switch (dtype) {
  case DATA_TYPE::BTYE:
  case DATA_TYPE::UNSIGNED_BYTE:
  case DATA_TYPE::SHORT:
  case DATA_TYPE::UNSIGNED_SHORT:
  case DATA_TYPE::INT:
  case DATA_TYPE::UNSIGNED_INT:
    glVertexAttribIPointer(index, size, (GLenum)dtype, stride,
                           (const void *)(ptrdiff_t)offset);
    break;
  case DATA_TYPE::HALF_FLOAT:
  case DATA_TYPE::FLOAT:
    glVertexAttribPointer(index, size, (GLenum)dtype, stride, normalized,
                          (const void *)(ptrdiff_t)offset);
    break;
  case DATA_TYPE::DOUBLE:
    glVertexAttribLPointer(index, size, (GLenum)dtype, stride,
                           (const void *)(ptrdiff_t)offset);
    break;
  case DATA_TYPE::FIXED:
    glVertexAttribPointer(index, size, (GLenum)dtype, stride, normalized,
                          (const void *)(ptrdiff_t)offset);
    break;
  case DATA_TYPE::INT_2_10_10_10_REV:
  case DATA_TYPE::UNSIGNED_INT_2_10_10_10_REV:
  case DATA_TYPE::UNSIGNED_INT_10F_11F_11F_REV:
    glVertexAttribIPointer(index, size, (GLenum)dtype, stride,
                           (const void *)(ptrdiff_t)offset);
    break;
  }
}
void VertexArray::enableAttribute(const uint32_t &index) {
  glEnableVertexAttribArray(index);
}
void VertexArray::disableAttribute(const uint32_t &index) {
  glDisableVertexAttribArray(index);
}
void VertexArray::bind(const core::AutoPtr<VertexArray> &vao) {
  glBindVertexArray(vao->_handle);
}