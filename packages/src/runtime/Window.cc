#include "runtime/Window.hpp"
#include <SDL2/SDL.h>
#include <SDL_mouse.h>
#include <SDL_video.h>
#include <glad/glad.h>
#include <stdexcept>

using namespace firefly;
using namespace firefly::runtime;
#define SDL_ASSERT(expr)                                                       \
  if (!(expr))                                                                 \
  throw std::runtime_error(SDL_GetError())

Window::Window(const std::string &title, int width, int height) {
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) == 0);
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2) == 0);
#ifndef __apple__
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                 SDL_GL_CONTEXT_PROFILE_CORE) == 0);
#else
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                 SDL_GL_CONTEXT_PROFILE_CORE |
                                     SDL_GL_CONTEXT_PROFILE_COMPATIBILITY));
#endif
  _window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width, height,
                             SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL |
                                 SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
  SDL_ASSERT(_window != nullptr);
  _ctx = SDL_GL_CreateContext(_window);
  SDL_ASSERT(_ctx != nullptr);
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw std::runtime_error("Failed to initialize glad");
  }
}

void Window::setTitle(const std::string &title) {
  SDL_SetWindowTitle(_window, title.c_str());
}

const std::string Window::getTitle() const {
  return SDL_GetWindowTitle(_window);
}

void Window::present() { SDL_GL_SwapWindow(_window); }

void Window::setSwapInterval(int flag) {
  SDL_ASSERT(SDL_GL_SetSwapInterval(flag) >= 0);
}
const glm::vec2 Window::getSize() const {
  int w, h;
  SDL_GetWindowSize(_window, &w, &h);
  return {w, h};
}
void Window::setSize(const glm::vec2 &size) {
  SDL_SetWindowSize(_window, size.x, size.y);
}
void Window::hide() { SDL_HideWindow(_window); }
void Window::show() { SDL_ShowWindow(_window); }