#include "document/Window.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/ResizeEvent.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
using namespace firefly;
using namespace firefly::document;

Window::Window()
    : _window(nullptr), _title("firefly - [no name]"), _position(-1, -1),
      _size(1024, 768), _fullscreen(false) {
  defineProperty(PROP_TITLE, _title);
  defineProperty(PROP_POSITION, _position);
  defineProperty(PROP_SIZE, _size);
  defineProperty(PROP_FULLSCREEN, _fullscreen);
}

void Window::onLoad() {
  auto [x, y] = _position;
  auto [width, height] = _size;
  if (x == -1) {
    x = SDL_WINDOWPOS_CENTERED;
  }
  if (y == -1) {
    y = SDL_WINDOWPOS_CENTERED;
  }
  core::Unsigned_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
  if (_fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    x = 0;
    y = 0;
  } else {
    flags |= SDL_WINDOW_RESIZABLE;
  }
  _window = SDL_CreateWindow(_title.c_str(), x, y, width, height, flags);
  _renderer = new video::Renderer(_window);
  _eventBus->on(this, &Window::onResize);
  Node::onLoad();
}

void Window::onTick() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  Node::onTick();

  _renderer->present();
  SDL_GL_SwapWindow(_window);
}

void Window::onUnload() {
  _renderer = nullptr;
  _eventBus->off<Window, runtime::ResizeEvent>(this);
  SDL_DestroyWindow(_window);
  _window = nullptr;
}

void Window::onResize(runtime::ResizeEvent &e) {
  if (e.getWindowId() == SDL_GetWindowID(_window)) {
    _renderer->setViewport({0, 0, e.getSize()});
  }
}

const core::AutoPtr<video::Renderer> &Window::getRenderer() const {
  return _renderer;
}

core::AutoPtr<video::Renderer> &Window::getRenderer() { return _renderer; }

core::Unsigned_t Window::getWindowID() const {
  return SDL_GetWindowID((SDL_Window *)_window);
}