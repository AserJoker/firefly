#pragma once
#include "core/Object.hpp"
#include <SDL2/SDL.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>

namespace firefly::runtime {
class Window : public core::Object {
private:
  SDL_Window *_window;
  SDL_GLContext _ctx;

public:
  Window(const std::string &title, int width, int height);
  void setTitle(const std::string &title);
  const std::string getTitle() const;
  const glm::vec2 getSize() const;
  void setSize(const glm::vec2 &size);
  void present();
  void setSwapInterval(int flag);
  void hide();
  void show();
};
} // namespace firefly::runtime