#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Camera : public core::Object {
public:
  virtual glm::mat4 getMatrix() = 0;
};
} // namespace firefly::video