#include "gl/VertexArray.hpp"
using namespace firefly;
using namespace firefly::gl;

VertexArray::VertexArray(core::Unsigned_t handle) : _handle(handle) {
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
void VertexArray::setAttribute(core::Unsigned_t index, DATA_TYPE dtype,
                               core::Unsigned_t size,
                               core::Boolean_t normalized,
                               core::Unsigned_t stride,
                               core::Unsigned_t offset) {
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
    glVertexAttribPointer(index, size, (GLenum)dtype, normalized, stride,
                          (const void *)(ptrdiff_t)offset);
    break;
  case DATA_TYPE::DOUBLE:
    glVertexAttribLPointer(index, size, (GLenum)dtype, stride,
                           (const void *)(ptrdiff_t)offset);
    break;
  case DATA_TYPE::FIXED:
    glVertexAttribPointer(index, size, (GLenum)dtype, normalized, stride,
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
void VertexArray::enableAttribute(core::Unsigned_t index) {
  glEnableVertexAttribArray(index);
}
void VertexArray::disableAttribute(core::Unsigned_t index) {
  glDisableVertexAttribArray(index);
}
void VertexArray::setVertexAttribDivisor(core::Unsigned_t index,
                                         core::Unsigned_t divisor) {
  glVertexAttribDivisor(index, divisor);
}
void VertexArray::bind(const core::AutoPtr<VertexArray> &vao) {
  glBindVertexArray(vao->_handle);
}