#include "video/OrthoCamera.hpp"
#include "runtime/Application.hpp"
#include "video/Camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::video;
glm::mat4 OrthoCamera::getProjectionMatrix(const glm::ivec4 &viewport) {
  auto x = viewport.x;
  auto y = viewport.y;
  auto width = viewport.z;
  auto height = viewport.w;
  auto left = ((float)x - width / 2.0f) / width;
  auto right = ((float)x + width / 2.0f) / width;
  auto bottom = ((float)y - height / 2.0f) / height;
  auto top = ((float)y + height / 2.0f) / height;

  auto projection = glm::ortho(left, right, bottom, top, -1000.f, 1000.0f) *
                    glm::scale(glm::mat4(1.0f), {1.0f / width, 1.0f / height, 1.0f});
  return projection;
}
OrthoCamera::OrthoCamera(const glm::vec3 &position, const glm::vec3 &up,
                         const glm::vec3 &front)
    : Camera(glm::mat4(1.0f), position, up, front) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  auto size = win->getSize();
  setViewport({0, 0, size});
}

void OrthoCamera::setViewport(const glm::ivec4 &viewport) {
  if (_viewport != viewport) {
    setProjectionMatrix(getProjectionMatrix(viewport));
    _viewport = viewport;
  }
}