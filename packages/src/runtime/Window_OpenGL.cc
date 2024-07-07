#include "runtime/Window_OpenGL.hpp"
#include "core/AutoPtr.hpp"
#include "video/Renderer_OpenGL.hpp"
#include <SDL.h>
#include <glad/glad.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::runtime;
#define SDL_ASSERT(expr)                                                       \
  if (!(expr))                                                                 \
  throw std::runtime_error(SDL_GetError())
Window_OpenGL::Window_OpenGL(const std::string &title, int width, int height) {
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
                             SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
  SDL_ASSERT(_window != nullptr);
  _ctx = SDL_GL_CreateContext(_window);
  SDL_ASSERT(_ctx != nullptr);
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw std::runtime_error("Failed to initialize glad");
  }
}
void Window_OpenGL::setTitle(const std::string &title) {
  SDL_SetWindowTitle(_window, title.c_str());
}
std::string Window_OpenGL::getTitle() const {
  return SDL_GetWindowTitle(_window);
}
void Window_OpenGL::present() { SDL_GL_SwapWindow(_window); }
core::AutoPtr<video::IRenderer> Window_OpenGL::createRenderer() {
  return new video::Renderer_OpenGL();
}