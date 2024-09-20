#pragma once
#include "video/Camera.hpp"
namespace firefly::video {
class OrthoCamera : public Camera {
public:
  OrthoCamera(const glm::ivec4 &viewport,
              const glm::vec3 &position = {0.f, 0.f, 0.f},
              const glm::vec3 &up = {0.f, 1.f, 0.f},
              const glm::vec3 &front = {0.f, 0.f, 1.f});
  void setViewport(const glm::ivec4 &viewport) override;
};
}; // namespace firefly::video