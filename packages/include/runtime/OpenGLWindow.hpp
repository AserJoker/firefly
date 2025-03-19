#pragma once
#include "runtime/Window.hpp"
#include <SDL_video.h>
namespace firefly::runtime {
class OpenGLWindow : public Window {
private:
  SDL_GLContext _glContext;

public:
  OpenGLWindow(const std::string &title, uint32_t width, uint32_t height,
               int32_t x = -1, int32_t y = -1, int flags = SDL_WINDOW_SHOWN);
  ~OpenGLWindow();
};
} // namespace firefly::runtime