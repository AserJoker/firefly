#include "video/PerspectiveCamera.hpp"
#include "video/Camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>
using namespace firefly;
using namespace firefly::video;
PerspectiveCamera::PerspectiveCamera(const glm::ivec2 &size,
                                     const glm::vec3 &position,
                                     const glm::vec3 &up,
                                     const glm::vec3 &front)
    : Camera(glm::mat4(1.0f), position, up, front) {
  setProjectionMatrix(glm::perspective(
      glm::radians(45.0f), (float)size.x / (float)size.y, 0.1f, 100.0f));
}
void PerspectiveCamera::setViewport(const glm::ivec4 &viewport) {
  setProjectionMatrix(glm::perspective(glm::radians(45.0f),
                                       (float)viewport.z / (float)viewport.w,
                                       0.1f, 100.0f));
}