#pragma once
#include "ElementType.hpp"
#include "PrimitiveType.hpp"
#include "core/Object.hpp"
namespace firefly::gl {
class Device : public core::Object {
public:
  void drawArrays(PRIMITIVE_TYPE mode, uint32_t first, size_t count);

  void drawElements(PRIMITIVE_TYPE mode, int32_t count, ELEMENT_TYPE type,
                    const void *indices);

  void multiDrawArrays(PRIMITIVE_TYPE mode, const int32_t *first,
                       const int32_t *count, size_t drawcount);

  void multiDrawElements(PRIMITIVE_TYPE mode, const int32_t *count,
                         ELEMENT_TYPE type, const void **indices,
                         size_t drawcount);

  void drawElementsBaseVertex(PRIMITIVE_TYPE mode, int32_t count,
                              ELEMENT_TYPE type, const void *indices,
                              uint32_t basevertex);

  void drawArraysInstanced(PRIMITIVE_TYPE mode, uint32_t first, size_t count,
                           size_t instancecount);

  void drawElementsInstanced(PRIMITIVE_TYPE mode, int32_t count,
                             ELEMENT_TYPE type, const void *indices,
                             size_t instancecount);

  void drawArraysInstancedBaseInstance(PRIMITIVE_TYPE mode, uint32_t first,
                                       size_t count, size_t instancecount,
                                       size_t basedinstance);

  void drawElementsInstancedBaseInstance(PRIMITIVE_TYPE mode, uint32_t count,
                                         ELEMENT_TYPE type, const void *indices,
                                         size_t instancecount,
                                         size_t baseinstance);

  void drawRangeElements(PRIMITIVE_TYPE mode, uint32_t start, uint32_t end,
                         int32_t count, ELEMENT_TYPE type, const void *indices);

  void multiDrawElementsBaseVertex(PRIMITIVE_TYPE mode, const int32_t *count,
                                   ELEMENT_TYPE type, const void **indices,
                                   size_t primcount, const int32_t *basevertex);

  void drawRangeElementsBaseVertex(PRIMITIVE_TYPE mode, uint32_t start,
                                   uint32_t end, int32_t count,
                                   ELEMENT_TYPE type, const void *indices,
                                   int32_t basevertex);

  void drawElementsInstancedBaseVertex(PRIMITIVE_TYPE mode, int32_t count,
                                       ELEMENT_TYPE type, const void *indices,
                                       int32_t instancecount,
                                       int32_t basevertex);

  void drawElementsInstancedBaseVertexBaseInstance(
      PRIMITIVE_TYPE mode, int32_t count, ELEMENT_TYPE type,
      const void *indices, int32_t instancecount, int32_t basevertex,
      int32_t basedinstance);

  void drawTransformFeedback(PRIMITIVE_TYPE mode, uint32_t id);

  void drawTransformFeedbackStream(PRIMITIVE_TYPE mode, uint32_t id,
                                   uint32_t stream);

  void drawTransformFeedbackInstanced(PRIMITIVE_TYPE mode, uint32_t id,
                                      int32_t instancecount);

  void drawTransformFeedbackStreamInstanced(PRIMITIVE_TYPE mode, uint32_t id,
                                            uint32_t stream,
                                            int32_t instancecount);

  void drawArrayIndirect(PRIMITIVE_TYPE mode, const void *indirect);

  void drawElementsIndirect(PRIMITIVE_TYPE mode, ELEMENT_TYPE type,
                            const void *indirect);

  void multiDrawArraysIndirect(PRIMITIVE_TYPE mode, const void *indirect,
                               int32_t drawcount, int32_t stride);

  void multiDrawElementsIndirect(PRIMITIVE_TYPE mode, ELEMENT_TYPE type,
                                 const void *indirect, int32_t drawcount,
                                 int32_t stride);

public:
  static bool checkVersion(int32_t major, int32_t minor);
  static std::string getVersion();
};
#define CHECK_VERSION(major, minor, msg)                                       \
  if (!Device::checkVersion(major, minor)) {                                   \
    throw exception::OpenGLVersionException(                                   \
        fmt::format("{}: current OpenGL version is {},require 'v{}.{}'", msg,  \
                    Device::getVersion(), major, minor));                      \
  }
}; // namespace firefly::gl