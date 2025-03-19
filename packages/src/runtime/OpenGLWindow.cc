#include "runtime/OpenGLWindow.hpp"
#include "core/Exception.hpp"
using namespace firefly;
using namespace firefly::runtime;
OpenGLWindow::OpenGLWindow(const std::string &title, uint32_t width,
                           uint32_t height, int32_t x, int32_t y, int flags)
    : Window(title, width, height, x, y, flags | SDL_WINDOW_OPENGL) {
  _glContext = SDL_GL_CreateContext((SDL_Window *)getHandle());
  if (!_glContext) {
    throw core::Exception(
        fmt::format("Failed to create OpenGL context:{}", SDL_GetError()));
  }
  if (SDL_GL_MakeCurrent((SDL_Window *)getHandle(), _glContext) < 0) {
    throw core::Exception(
        fmt::format("Failed to bind OpenGL context:{}", SDL_GetError()));
  }
}

OpenGLWindow::~OpenGLWindow() { SDL_GL_DeleteContext(_glContext); }