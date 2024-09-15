#include "video/PerspectiveCamera.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_Resize.hpp"
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
  setProjectionMatrix(glm::perspective(
      glm::radians(45.0f), (float)size.x / (float)size.y, 0.1f, 100.0f));
  auto bus = core::Singleton<runtime::EventBus>::instance();
  bus->on(this, &PerspectiveCamera::onWindowResize);
}
void PerspectiveCamera::onWindowResize(const runtime::Event_Resize &event) {
  auto size = event.getSize();
  setProjectionMatrix(glm::perspective(
      glm::radians(45.0f), (float)size.x / (float)size.y, 0.1f, 100.0f));
}