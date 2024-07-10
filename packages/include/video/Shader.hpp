#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "runtime/Resource.hpp"
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
  Shader(const std::map<ShaderType, core::AutoPtr<runtime::Resource>> &sources);
  ~Shader() override;
  void use() const;
  void setValue(const std::string &name, const int32_t &val) const;
  void setValue(const std::string &name, const uint32_t &val) const;
  void setValue(const std::string &name, const float &val) const;
  void setValue(const std::string &name, const glm::vec2 &val) const;
  void setValue(const std::string &name, const float &x, const float &y) const;
  void setValue(const std::string &name, const glm::vec3 &val) const;
  void setValue(const std::string &name, const float &x, const float &y,
                const float &z) const;
  void setValue(const std::string &name, const glm::vec4 &val) const;
  void setValue(const std::string &name, const float &x, const float &y,
                const float &z, const float &w) const;
  void setValue(const std::string &name, const glm::mat2 &val) const;
  void setValue(const std::string &name, const glm::mat3 &val) const;
  void setValue(const std::string &name, const glm::mat4 &val) const;
};
} // namespace firefly::video