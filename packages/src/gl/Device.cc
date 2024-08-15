#include "gl/Device.hpp"
#include "exception/OpenGLVersionException.hpp"
#include "gl/OpenGLException.hpp"
#include <fmt/core.h>
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
using exception::OpenGLVersionException;
void Device::drawArrays(PRIMITIVE_TYPE mode, uint32_t first, size_t count) {
  glDrawArrays((GLenum)mode, first, count);
  CHECK_OPENGL("Failed to draw arrays");
}

void Device::drawElements(PRIMITIVE_TYPE mode, int32_t count, ELEMENT_TYPE type,
                          const void *indices) {
  glDrawElements((GLenum)mode, count, (GLenum)type, indices);
  CHECK_OPENGL("Failed to draw elements");
}

void Device::multiDrawArrays(PRIMITIVE_TYPE mode, const int32_t *first,
                             const int32_t *count, size_t drawcount) {
  glMultiDrawArrays((GLenum)mode, first, count, drawcount);
  CHECK_OPENGL("Failed to multi draw arrays");
}

void Device::multiDrawElements(PRIMITIVE_TYPE mode, const int32_t *count,
                               ELEMENT_TYPE type, const void **indices,
                               size_t drawcount) {
  glMultiDrawElements((GLenum)mode, count, (GLenum)type, indices, drawcount);
  CHECK_OPENGL("Failed to multi draw elements");
}

void Device::drawElementsBaseVertex(PRIMITIVE_TYPE mode, int32_t count,
                                    ELEMENT_TYPE type, const void *indices,
                                    uint32_t basevertex) {
  CHECK_VERSION(3, 2, "Failed to draw elements base vertex");
  glDrawElementsBaseVertex((GLenum)mode, count, (GLenum)type, indices,
                           basevertex);
  CHECK_OPENGL("Failed to draw elements base vertex");
}

void Device::drawArraysInstanced(PRIMITIVE_TYPE mode, uint32_t first,
                                 size_t count, size_t instancecount) {
  CHECK_VERSION(3, 1, "Failed to draw arrays instanced");
  glDrawArraysInstanced((GLenum)mode, first, count, instancecount);
  CHECK_OPENGL("Failed to draw arrays instanced");
}

void Device::drawElementsInstanced(PRIMITIVE_TYPE mode, int32_t count,
                                   ELEMENT_TYPE type, const void *indices,
                                   size_t instancecount) {
  CHECK_VERSION(3, 1, "Failed to draw elements instanced");
  glDrawElementsInstanced((GLenum)mode, count, (GLenum)type, indices,
                          instancecount);
  CHECK_OPENGL("Failed to draw elements instanced");
}

void Device::drawArraysInstancedBaseInstance(PRIMITIVE_TYPE mode,
                                             uint32_t first, size_t count,
                                             size_t instancecount,
                                             size_t basedinstance) {
  CHECK_VERSION(4, 2, "Failed to draw arrays instanced base instance");
  glDrawArraysInstancedBaseInstance((GLenum)mode, first, count, instancecount,
                                    basedinstance);
  CHECK_OPENGL("Failed to draw arrays instanced base instance");
}

void Device::drawElementsInstancedBaseInstance(
    PRIMITIVE_TYPE mode, uint32_t count, ELEMENT_TYPE type, const void *indices,
    size_t instancecount, size_t baseinstance) {
  CHECK_VERSION(4, 2, "Failed to draw elements instanced based instance");
  glDrawElementsInstancedBaseInstance((GLenum)mode, count, (GLenum)type,
                                      indices, instancecount, baseinstance);
  CHECK_OPENGL("Failed to draw elements instanced based instance");
}

void Device::drawRangeElements(PRIMITIVE_TYPE mode, uint32_t start,
                               uint32_t end, int32_t count, ELEMENT_TYPE type,
                               const void *indices) {
  glDrawRangeElements((GLenum)mode, start, end, count, (GLenum)type, indices);
  CHECK_OPENGL("Failed to draw range elements");
}

void Device::multiDrawElementsBaseVertex(PRIMITIVE_TYPE mode,
                                         const int32_t *count,
                                         ELEMENT_TYPE type,
                                         const void **indices, size_t drawcount,
                                         const int32_t *basevertex) {
  CHECK_VERSION(3, 2, "Failed to multi draw elements base vertex");
  glMultiDrawElementsBaseVertex((GLenum)mode, count, (GLenum)type, indices,
                                drawcount, basevertex);
  CHECK_OPENGL("Failed to multi draw elements base vertex");
}

void Device::drawRangeElementsBaseVertex(PRIMITIVE_TYPE mode, uint32_t start,
                                         uint32_t end, int32_t count,
                                         ELEMENT_TYPE type, const void *indices,
                                         int32_t basevertex) {
  CHECK_VERSION(3, 2, "Failed to draw range elements base vertex");
  glDrawRangeElementsBaseVertex((GLenum)mode, start, end, count, (GLenum)type,
                                indices, basevertex);
  CHECK_OPENGL("Failed to draw range elements base vertex");
}

void Device::drawElementsInstancedBaseVertex(PRIMITIVE_TYPE mode, int32_t count,
                                             ELEMENT_TYPE type,
                                             const void *indices,
                                             int32_t instancecount,
                                             int32_t basevertex) {
  CHECK_VERSION(3, 2, "Failed to draw elements instanced base vertex");
  glDrawElementsInstancedBaseVertex((GLenum)mode, count, (GLenum)type, indices,
                                    instancecount, basevertex);
  CHECK_OPENGL("Failed to draw elements instanced base vertex");
}

void Device::drawElementsInstancedBaseVertexBaseInstance(
    PRIMITIVE_TYPE mode, int32_t count, ELEMENT_TYPE type, const void *indices,
    int32_t instancecount, int32_t basevertex, int32_t baseinstance) {
  CHECK_VERSION(4, 2,
                "Failed to draw elements instanced base vertex base instance");
  glDrawElementsInstancedBaseVertexBaseInstance(
      (GLenum)mode, count, (GLenum)type, indices, instancecount, basevertex,
      baseinstance);
  CHECK_OPENGL("Failed to draw elements instanced base vertex base instance");
}

void Device::drawTransformFeedback(PRIMITIVE_TYPE mode, uint32_t id) {
  CHECK_VERSION(4, 0, "Failed to draw transform feedback");
  glDrawTransformFeedback((GLenum)mode, id);
  CHECK_OPENGL("Failed to draw transform feedback");
}

void Device::drawTransformFeedbackStream(PRIMITIVE_TYPE mode, uint32_t id,
                                         uint32_t stream) {
  CHECK_VERSION(4, 0, "Failed to draw transform feedback stream");
  glDrawTransformFeedbackStream((GLenum)mode, id, stream);
  CHECK_OPENGL("Failed to draw transform feedback stream");
}

void Device::drawTransformFeedbackInstanced(PRIMITIVE_TYPE mode, uint32_t id,
                                            int32_t instancecount) {
  CHECK_VERSION(4, 2, "Failed to draw transform feedback instanced");
  glDrawTransformFeedbackInstanced((GLenum)mode, id, instancecount);
  CHECK_OPENGL("Failed to draw transform feedback instanced");
}

void Device::drawTransformFeedbackStreamInstanced(PRIMITIVE_TYPE mode,
                                                  uint32_t id, uint32_t stream,
                                                  int32_t instancecount) {

  CHECK_VERSION(4, 2, "Failed to draw transform feedback stream instanced");
  glDrawTransformFeedbackStreamInstanced((GLenum)mode, id, stream,
                                         instancecount);
  CHECK_OPENGL("Failed to draw transform feedback stream instanced");
}

void Device::drawArrayIndirect(PRIMITIVE_TYPE mode, const void *indirect) {
  CHECK_VERSION(4, 0, "Failed to draw arrays indirect");
  glDrawArraysIndirect((GLenum)mode, indirect);
  CHECK_OPENGL("Failed to draw arrays indirect");
}

void Device::drawElementsIndirect(PRIMITIVE_TYPE mode, ELEMENT_TYPE type,
                                  const void *indirect) {
  CHECK_VERSION(4, 0, "Failed to draw elements indirect");
  glDrawElementsIndirect((GLenum)mode, (GLenum)type, indirect);
  CHECK_OPENGL("Failed to draw elements indirect");
}

void Device::multiDrawArraysIndirect(PRIMITIVE_TYPE mode, const void *indirect,
                                     int32_t drawcount, int32_t stride) {
  CHECK_VERSION(4, 3, "Failed to multi draw arrays indirect");
  glMultiDrawArraysIndirect((GLenum)mode, indirect, drawcount, stride);
  CHECK_OPENGL("Failed to multi draw arrays indirect");
}

void Device::multiDrawElementsIndirect(PRIMITIVE_TYPE mode, ELEMENT_TYPE type,
                                       const void *indirect, int32_t drawcount,
                                       int32_t stride) {
  CHECK_VERSION(4, 3, "Failed to multi draw arrays indirect");
  glMultiDrawElementsIndirect((GLenum)mode, (GLenum)type, indirect, drawcount,
                              stride);
  CHECK_OPENGL("Failed to multi draw arrays indirect");
}

bool Device::checkVersion(int32_t major, int32_t minor) {
  static int32_t currentMajor = 0;
  static int32_t currentMinor = 0;
  if (currentMajor == 0) {
    glGetIntegerv(GL_MAJOR_VERSION, &currentMajor);
  }
  if (currentMinor == 0) {
    glGetIntegerv(GL_MINOR_VERSION, &currentMinor);
  }
  if (currentMajor > major) {
    return true;
  } else if (currentMajor < major) {
    return false;
  } else {
    if (currentMinor >= minor) {
      return true;
    }
    return false;
  }
}
std::string Device::getVersion() {
  static std::string version;
  if (version.empty()) {
    int32_t currentMajor = 0;
    int32_t currentMinor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &currentMajor);
    glGetIntegerv(GL_MINOR_VERSION, &currentMinor);
    version = fmt::format("v{}.{}", currentMajor, currentMinor);
  }
  return version;
}