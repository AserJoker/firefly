#include "document/Window.hpp"
#include "runtime/Event_Resize.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
using namespace firefly;
using namespace firefly::document;

Window::Window() : _window(nullptr) {
  _window = SDL_CreateWindow("firefly - [no name]", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 1024, 768,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                 SDL_WINDOW_ALLOW_HIGHDPI);
  _renderer = new video::Renderer(_window);
  _eventBus->on(this, &Window::onResize);
  provide(_renderer);
}

void Window::onTick() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  Node::onTick();
  _renderer->present();
  SDL_GL_SwapWindow(_window);
}

void Window::onResize(runtime::Event_Resize &e) {
  if (e.getWindowId() == SDL_GetWindowID(_window)) {
    _renderer->setViewport({0, 0, e.getSize()});
  }
}