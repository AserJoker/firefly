#pragma once
#include "video/Camera.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class OrthoCamera : public Camera {
private:
  static glm::mat4 getProjectionMatrix(const glm::ivec4 &viewport);

public:
  OrthoCamera(const glm::vec3 &position = {0.f, 0.f, 0.f},
              const glm::vec3 &up = {0.f, 1.f, 0.f},
              const glm::vec3 &front = {0.f, 0.f, 1.f});
  void setViewport(const glm::ivec4 &viewport) override;
};
}; // namespace firefly::video