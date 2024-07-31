#pragma once
#include "core/Object.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
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
  void setUniform(const std::string &name, float value);
  void setUniform(const std::string &name, int value);
  void setUniform(const std::string &name, bool value);
  void setUniform(const std::string &name, const glm::vec2 &value);
  void setUniform(const std::string &name, const glm::vec3 &value);
  void setUniform(const std::string &name, const glm::vec4 &value);
  void setUniform(const std::string &name, const glm::mat3x3 &value);
  void setUniform(const std::string &name, const glm::mat4x4 &value);
};
}; // namespace firefly::video