#pragma once
#include "Uniform.hpp"
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "gl/Shader.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
namespace firefly::gl {
class Program : public core::Object, public core::Cache<Program> {
private:
  uint32_t _handle;
  std::unordered_map<std::string, uint32_t> _locations;
  std::unordered_map<std::string, Uniform> _uniforms;

private:
  const uint32_t &getUniformLocation(const std::string &name);
  void setUniform(const std::string &name, int32_t value);
  void setUniform(const std::string &name, bool value);
  void setUniform(const std::string &name, float value);
  void setUniform(const std::string &name, const glm::vec2 &value);
  void setUniform(const std::string &name, const glm::vec3 &value);
  void setUniform(const std::string &name, const glm::vec4 &value);
  void setUniform(const std::string &name, const glm::mat2 &value,
                  const bool &transpose = false);
  void setUniform(const std::string &name, const glm::mat2x3 &value,
                  const bool &transpose = false);
  void setUniform(const std::string &name, const glm::mat3x2 &value,
                  const bool &transpose = false);
  void setUniform(const std::string &name, const glm::mat3 &value,
                  const bool &transpose = false);
  void setUniform(const std::string &name, const glm::mat3x4 &value,
                  const bool &transpose = false);
  void setUniform(const std::string &name, const glm::mat4x3 &value,
                  const bool &transpose = false);
  void setUniform(const std::string &name, const glm::mat4 &value,
                  const bool &transpose = false);

public:
  Program();
  ~Program() override;
  void attach(const core::AutoPtr<Shader> &shader);
  bool link();
  const std::string getInfoLog() const;
  const uint32_t &getHandle() const;
  void use();
  void setUniform(const std::string &name, const Uniform &uniform);
};
}; // namespace firefly::gl