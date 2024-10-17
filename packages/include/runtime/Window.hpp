#pragma once
#include "core/Object.hpp"
#include "core/Rect.hpp"
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
  const core::Size<uint32_t> getSize() const;
  void setSize(const core::Size<uint32_t> &size);
  const core::Point<int32_t> getWindowPosition() const;
  void setWindowPosition(const core::Point<int32_t> &pos);
  void present();
  void setSwapInterval(int flag);
  void hide();
  void show();
};
} // namespace firefly::runtime