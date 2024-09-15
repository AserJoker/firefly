#pragma once
#include "runtime/Event_Resize.hpp"
#include "video/Camera.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace firefly::video {
class PerspectiveCamera : public Camera {
private:
  void onWindowResize(const runtime::Event_Resize &event);

public:
  PerspectiveCamera(const glm::vec3 &position = {0.f, 0.f, 0.f},
                    const glm::vec3 &up = {0.f, 1.f, 0.f},
                    const glm::vec3 &front = {0.f, 0.f, 1.f});
};
} // namespace firefly::video