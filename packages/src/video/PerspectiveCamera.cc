#include "video/PerspectiveCamera.hpp"
#include "runtime/Application.hpp"
#include "video/Camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>
using namespace firefly;
using namespace firefly::video;
PerspectiveCamera::PerspectiveCamera(const glm::vec3 &position,
                                     const glm::vec3 &up,
                                     const glm::vec3 &front)
    : Camera(glm::mat4(1.0f), position, up, front) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  auto size = win->getSize();
  setViewport({0, 0, size});
}
void PerspectiveCamera::setViewport(const glm::ivec4 &viewport) {
  if (_viewport != viewport) {
    setProjectionMatrix(glm::perspective(glm::radians(45.0f),
                                         (float)viewport.z / (float)viewport.w,
                                         0.1f, 100.0f));
    _viewport = viewport;
  }
}