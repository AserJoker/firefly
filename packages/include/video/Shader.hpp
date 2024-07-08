#pragma once
#include "core/Object.hpp"
#include <glad/glad.h>
#include <map>
#include <string>
namespace firefly::video {
enum class ShaderType {
  COMPUTE = GL_COMPUTE_SHADER,
  VERTEX = GL_VERTEX_SHADER,
  TESS_CONTROL = GL_TESS_CONTROL_SHADER,
  TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
  GEOMETRY = GL_GEOMETRY_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER
};
class Shader : public core::Object {
private:
  uint32_t _handle;

public:
  Shader(const std::map<ShaderType, std::string> &sources);
  ~Shader() override;
  void use();
};
} // namespace firefly::video