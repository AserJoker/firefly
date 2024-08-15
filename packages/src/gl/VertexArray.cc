#include "gl/VertexArray.hpp"
#include "exception/OpenGLVersionException.hpp"
#include "gl/Device.hpp"
#include "gl/OpenGLException.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
using exception::OpenGLVersionException;
VertexArray::VertexArray(uint32_t handle)
    : _handle(handle), _autoDelete(false) {
  if (handle == 0) {
    glGenVertexArrays(1, &_handle);
    _autoDelete = true;
  }
}

VertexArray::~VertexArray() {
  if (_autoDelete) {
    glDeleteVertexArrays(1, &_handle);
  }
}

void VertexArray::addBuffer(BUFFER_TARGET target,
                            core::AutoPtr<Buffer> buffer) {
  VertexArray::bind(this);
  Buffer::bind(target, buffer);
  VertexArray::unbind();
}

void VertexArray::removeBuffer(BUFFER_TARGET target) {
  VertexArray::bind(this);
  Buffer::unbind(target);
  VertexArray::unbind();
}

void VertexArray::enable(uint32_t index) {
  VertexArray::bind(this);
  glEnableVertexAttribArray(index);
  CHECK_OPENGL("Faield to enable VertexArray Attrib");
  VertexArray::unbind();
}

void VertexArray::disable(uint32_t index) {
  VertexArray::bind(this);
  glDisableVertexAttribArray(index);
  CHECK_OPENGL("Faield to enable VertexArray Attrib");
  VertexArray::unbind();
}

void VertexArray::setAttribPointer(uint32_t index, uint32_t size,
                                   COMPOMENT_TYPE type, uint32_t stride,
                                   bool normalize, void *pointer) {
  VertexArray::bind(this);
  switch (type) {
  case COMPOMENT_TYPE::BYTE:
  case COMPOMENT_TYPE::UNSIGNED_BYTE:
  case COMPOMENT_TYPE::SHORT:
  case COMPOMENT_TYPE::INT:
  case COMPOMENT_TYPE::INT_2_10_10_10_REV:
  case COMPOMENT_TYPE::UNSIGNED_INT_2_10_10_10_REV:
  case COMPOMENT_TYPE::UNSIGNED_INT_10F_11F_11F_REV:
  case COMPOMENT_TYPE::HALF_FLOAt:
  case COMPOMENT_TYPE::FLOAT:
  case COMPOMENT_TYPE::FIXED:
    glVertexAttribPointer(index, size, (GLenum)type, normalize, stride,
                          pointer);
    break;
  case COMPOMENT_TYPE::DOUBLE:
    CHECK_VERSION(4, 1, "Failed to set attribute pointer");
    glVertexAttribLPointer(index, size, (GLenum)type, stride, pointer);
    break;
  }
  CHECK_OPENGL("Failed to set attribute pointer");
  VertexArray::unbind();
}
void *VertexArray::getAttribPointer(uint32_t index) {
  VertexArray::bind(this);
  void *output = nullptr;
  glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, &output);
  CHECK_OPENGL("Failed to get attribute pointer");
  VertexArray::unbind();
  return output;
}
void VertexArray::bindVertexBuffer(uint32_t bindingindex,
                                   core::AutoPtr<Buffer> buffer,
                                   uint32_t offset, size_t length) {
  VertexArray::bind(this);
  glBindVertexBuffer(bindingindex, buffer->getHandle(), offset, length);
  CHECK_OPENGL("Failed to bind vertex buffer");
  VertexArray::unbind();
}

void VertexArray::setAttribDivisor(uint32_t index, uint32_t divisor) {
  CHECK_VERSION(3, 3, "Failed to set attribute divisor");
  VertexArray::bind(this);
  glVertexAttribDivisor(index, divisor);
  CHECK_OPENGL("Failed to set attribute divisor");
  VertexArray::unbind();
}

void VertexArray::setBindingDivisor(uint32_t bindingindex, uint32_t divisor) {
  CHECK_VERSION(4, 3, "Failed to bind divisor");
  VertexArray::bind(this);
  glVertexBindingDivisor(bindingindex, divisor);
  CHECK_OPENGL("Failed to set bind divisor");
  VertexArray::unbind();
}

void VertexArray::setAttribBinding(uint32_t index, uint32_t bindingindex) {
  CHECK_VERSION(4, 3, "Failed to set attribute binding");
  VertexArray::bind(this);
  glVertexAttribBinding(index, bindingindex);
  CHECK_OPENGL("Failed to set attribute binding");
  VertexArray::unbind();
}
void VertexArray::setAttribFormat(uint32_t index, uint32_t size,
                                  COMPOMENT_TYPE type, bool normalize,
                                  uint32_t offset) {
  CHECK_VERSION(4, 3, "Failed to set attribute format");
  VertexArray::bind(this);
  glVertexAttribFormat(index, size, (GLenum)type, normalize, offset);
  CHECK_OPENGL("Failed to set attribute format");
  VertexArray::unbind();
}

void VertexArray::bind(core::AutoPtr<VertexArray> varr) {
  glBindVertexArray(varr->_handle);
  CHECK_OPENGL("Faield to bind VertexArray ");
}

void VertexArray::unbind() {
  glBindVertexArray(0);
  CHECK_OPENGL("Faield to unbind VertexArray ");
}