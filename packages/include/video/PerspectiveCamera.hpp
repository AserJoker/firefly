#pragma once
#include "core/Rect.hpp"
#include "video/Camera.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace firefly::video {
class PerspectiveCamera : public Camera {

public:
  PerspectiveCamera(const glm::vec3 &position = {0.f, 0.f, 0.f},
                    const glm::vec3 &up = {0.f, 1.f, 0.f},
                    const glm::vec3 &front = {0.f, 0.f, 1.f});
  void setViewport(const core::Rect<> &viewport) override;
};
} // namespace firefly::video