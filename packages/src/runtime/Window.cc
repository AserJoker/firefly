#include "runtime/Window.hpp"
#include "core/Exception.hpp"
#include <SDL_video.h>
using namespace firefly;
using namespace firefly::runtime;
Window::Window(const std::string &title, uint32_t width, uint32_t height,
               int32_t x, int32_t y, int flags) {
  _window = SDL_CreateWindow(
      title.c_str(), x == -1 ? SDL_WINDOWPOS_CENTERED : x,
      y == -1 ? SDL_WINDOWPOS_CENTERED : y, width, height, flags);
  if (!_window) {
    throw core::Exception(
        fmt::format("Failed to create window: {}", SDL_GetError()));
  }
}

Window::~Window() {
  SDL_DestroyWindow(_window);
  _window = nullptr;
}

const SDL_Window *Window::getHandle() const { return _window; }
