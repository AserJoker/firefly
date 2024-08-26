#pragma once
#include "core/Object.hpp"
#include <glm/fwd.hpp>
#include <unordered_map>
namespace firefly::gl {
class Shader : public core::Object {
private:
  uint32_t _handle;
  std::unordered_map<std::string, uint32_t> _locations;

public:
  Shader(const std::unordered_map<uint32_t, std::string> &sources,
         const uint32_t &handle = 0);
  ~Shader() override;
  const void use() const;
  const uint32_t &getHandle() const;
  void setValue(const std::string &name, const bool &value);
  void setValue(const std::string &name, const int32_t &value);
  void setValue(const std::string &name, const uint32_t &value);
  void setValue(const std::string &name, const float &value);
  void setValue(const std::string &name, const double &value);
  void setValue(const std::string &name, const glm::vec2 &value);
  void setValue(const std::string &name, const glm::vec3 &value);
  void setValue(const std::string &name, const glm::vec4 &value);
  void setValue(const std::string &name, const glm::mat2 &value);
  void setValue(const std::string &name, const glm::mat3 &value);
  void setValue(const std::string &name, const glm::mat4 &value);
  void setValue(const std::string &name, const glm::mat2x3 &value);
  void setValue(const std::string &name, const glm::mat3x2 &value);
  void setValue(const std::string &name, const glm::mat3x4 &value);
  void setValue(const std::string &name, const glm::mat4x3 &value);
};
}; // namespace firefly::gl