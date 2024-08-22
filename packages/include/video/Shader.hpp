#pragma once
#include "core/Object.hpp"
#include <glm/fwd.hpp>
#include <unordered_map>
namespace firefly::video {
class Shader : public core::Object {
private:
  uint32_t _handle;

public:
  Shader(const std::unordered_map<uint32_t, std::string> &sources,
         const uint32_t &handle = 0);
  ~Shader() override;
  void set(const std::string &name, const glm::mat3 &value);
  void set(const std::string &name, const glm::mat4 &value);
  const void use() const;
  const uint32_t &getHandle() const;
};
}; // namespace firefly::video