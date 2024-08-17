#pragma once
#include "PrimitiveType.hpp"
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <vector>
namespace firefly::gl {
class Program : public core::Object {
private:
  uint32_t _handle;

public:
  Program();
  ~Program() override;
  void attach(core::AutoPtr<Shader> shader);
  std::vector<core::AutoPtr<Shader>> getAttachedShaders();
  void detach(core::AutoPtr<Shader> shader);
  void use();
  void link();
  bool isDeleted();
  bool isLinkSuccess();
  bool isValidated();
  uint32_t getAttachShadersCount();
  uint32_t getActiveAtomicCounterBuffersCount();
  uint32_t getActiveAttributesCount();
  uint32_t getMaxActiveAttributeLength();
  uint32_t getActiveUniformsCount();
  uint32_t getMaxActiveUniformsCount();
  uint32_t getBinaryLength();
  uint32_t getComputeWorkGroupSize();
  uint32_t getTransformFeedbackBufferMode();
  uint32_t getTransformFeedbackVaryingVariablesCount();
  uint32_t getMaxTransformFeedbackVaryingVariablesCount();
  uint32_t getMaxGeometryOutputVerticesCount();
  PRIMITIVE_TYPE getGeometryInputType();
  PRIMITIVE_TYPE getGeometryOutputType();
};
} // namespace firefly::gl