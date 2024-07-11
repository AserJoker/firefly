#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Camera : public core::Object {
public:
  virtual const glm::mat4 &getViewMatrix() = 0;
  virtual const glm::mat4 &getProjectionMatrix() = 0;
};
} // namespace firefly::video