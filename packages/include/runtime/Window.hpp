#pragma once
#include "core/Object.hpp"
#include <SDL2/SDL.h>
#include <glm/fwd.hpp>
#include <string>
#include <glm/glm.hpp>

namespace firefly::runtime {
class Window : public core::Object {
private:
  SDL_Window *_window;
  SDL_GLContext _ctx;

public:
  Window(const std::string &title, int width, int height);
  void setTitle(const std::string &title);
  std::string getTitle() const;
  void present();
  void setSwapInterval(int flag);
  glm::vec2 getSize() const;
};
} // namespace firefly::runtime