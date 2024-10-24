#include "document/Camera2D.hpp"
#include "core/AutoPtr.hpp"
#include "core/Math.hpp"
#include "document/Window.hpp"
#include "runtime/ResizeEvent.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>

using namespace firefly;
using namespace firefly::document;
Camera2D::Camera2D() { _eventBus->on(this, &Camera2D::onResize); }
void Camera2D::onLoad() {
  auto win = findParent<Window>();
  auto renderer = win->getRenderer();
  auto viewport = renderer->getViewport();
  applyMatrix({viewport.width, viewport.height});
  renderer->setUniform("view", _view);
}
void Camera2D::setPosition(const core::Point<> &position) {
  _position = position;
  auto win = findParent<Window>();
  auto renderer = win->getRenderer();
  auto viewport = renderer->getViewport();
  applyMatrix({viewport.width, viewport.height});
  renderer->setUniform("view", _view);
}

void Camera2D::applyMatrix(const core::Size<> &size) {
  auto width = size.width / 2.0f;
  auto height = size.height / 2.0f;
  _view = core::translate({-_position.x * 1.0f, -_position.y * 1.0f, 0.0f}) *
          core::scale({1.0f, -1.0f, 1.0f}) *
          core::translate({-width, -height, 0.0f});
}

void Camera2D::onResize(runtime::ResizeEvent &e) {
  auto win = findParent<Window>();
  auto renderer = win->getRenderer();
  if (e.getWindowId() == SDL_GetWindowID((SDL_Window *)renderer->getWindow())) {
    applyMatrix(e.getSize());
    renderer->setUniform("view", _view);
  }
}