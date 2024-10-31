#include "document/Camera2D.hpp"
#include "core/AutoPtr.hpp"
#include "core/Math.hpp"
#include "document/Window.hpp"
#include "runtime/ResizeEvent.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>

using namespace firefly;
using namespace firefly::document;

Camera2D::Camera2D() {
  _eventBus->on(this, &Camera2D::onResize);
  defineProperty(PROP_POSITION, _position);

  watchProp(PROP_POSITION, &Camera2D::onPositionChange);
}

void Camera2D::onLoad() {
  _window = findParent<Window>();
  applyMatrix(_window->getRenderer()->getViewport().size);
  Node::onLoad();
}

void Camera2D::onUnload() {
  Node::onUnload();
  _window = nullptr;
}

void Camera2D::setPosition(const core::Point<> &position) {
  setProperty(PROP_POSITION, position);
}

void Camera2D::onPositionChange() {
  applyMatrix(_window->getRenderer()->getViewport().size);
}

void Camera2D::applyMatrix(const core::Size<> &size) {
  if (_window) {
    auto width = size.width / 2.0f;
    auto height = size.height / 2.0f;
    _view = core::translate({-_position.x * 1.0f, -_position.y * 1.0f, 0.0f}) *
            core::scale({1.0f, -1.0f, 1.0f}) *
            core::translate({-width, -height, 0.0f});
    _window->getRenderer()->setUniform("view", _view);
  }
}

void Camera2D::onResize(runtime::ResizeEvent &e) {
  if (e.getWindowId() == _window->getWindowID()) {
    applyMatrix(e.getSize());
  }
}