#pragma once
#include "core/Object.hpp"
#include <glm/fwd.hpp>
namespace firefly::video {
class Shader : public core::Object {
public:
  void set(const std::string &name, const glm::mat3 &value);
  void set(const std::string &name, const glm::mat4 &value);
  const void use() const;
};
}; // namespace firefly::video