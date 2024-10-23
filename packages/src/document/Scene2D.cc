#include "document/Scene2D.hpp"
#include "core/AutoPtr.hpp"
#include "core/Math.hpp"
#include "runtime/ResizeEvent.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>

using namespace firefly;
using namespace firefly::document;

Scene2D::Scene2D() { _eventBus->on(this, &Scene2D::onResize); }

void Scene2D::onLoad() {
  auto renderer = this->inject<core::AutoPtr<video::Renderer>>();
  if (renderer != nullptr) {
    auto viewport = renderer->getViewport();
    auto width = viewport.width / 2.0f;
    auto height = viewport.height / 2.0f;
    _projection = core::scale({1.0f / width, 1.0f / height, 0.0f});

    renderer->setShader("2d");
    renderer->setUniform("projection", _projection);
  }
}

void Scene2D::onResize(runtime::ResizeEvent &e) {
  auto &renderer = this->inject<core::AutoPtr<video::Renderer>>();
  if (e.getWindowId() == SDL_GetWindowID((SDL_Window *)renderer->getWindow())) {
    auto width = e.getSize().width / 2.0f;
    auto height = e.getSize().height / 2.0f;
    _projection = core::scale({1.0f / width, 1.0f / height, 0.0f});
    renderer->setUniform("projection", _projection);
  }
}