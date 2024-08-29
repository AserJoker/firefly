#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Shader.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
namespace firefly::gl {
class Program : public core::Object {
private:
  uint32_t _handle;
  std::unordered_map<std::string, uint32_t> _locations;
  std::unordered_map<std::string, uint32_t> _uniformBlockLocations;

public:
  Program();
  ~Program() override;
  void attach(const core::AutoPtr<Shader> &shader);
  bool link();
  const std::string getInfoLog() const;
  const uint32_t &getHandle() const;
  void use();
  const uint32_t &getUniformLocation(const std::string &name);
  void bindUniformBlock(const std::string &name, const uint32_t &index);
  void setUniform(const std::string &name, const int32_t &value);
  void setUniform(const std::string &name, const glm::ivec2 &value);
  void setUniform(const std::string &name, const glm::ivec3 &value);
  void setUniform(const std::string &name, const glm::ivec4 &value);
  void setUniform(const std::string &name, const uint32_t &value);
  void setUniform(const std::string &name, const glm::uvec2 &value);
  void setUniform(const std::string &name, const glm::uvec3 &value);
  void setUniform(const std::string &name, const glm::uvec4 &value);
  void setUniform(const std::string &name, const bool &value);
  void setUniform(const std::string &name, const float &value);
  void setUniform(const std::string &name, const glm::vec2 &value);
  void setUniform(const std::string &name, const glm::vec3 &value);
  void setUniform(const std::string &name, const glm::vec4 &value);
  void setUniform(const std::string &name, const double &value);
  void setUniform(const std::string &name, const glm::dvec2 &value);
  void setUniform(const std::string &name, const glm::dvec3 &value);
  void setUniform(const std::string &name, const glm::dvec4 &value);
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
};
}; // namespace firefly::gl