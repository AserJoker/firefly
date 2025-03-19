#pragma once
#include "core/Object.hpp"
#include <SDL2/SDL.h>
namespace firefly::runtime {
class Window : public core::Object {
private:
  SDL_Window *_window;

public:
  Window(const std::string &title, uint32_t width, uint32_t height,
         int32_t x = -1, int32_t y = -1, int flags = SDL_WINDOW_SHOWN);

  ~Window() override;

  const SDL_Window *getHandle() const;
};
} // namespace firefly::runtime