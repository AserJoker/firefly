#include "input/Mouse.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Event_MouseWheel.hpp"
#include "runtime/Event_SDL.hpp"
#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_video.h>
#include <glm/fwd.hpp>

using namespace firefly;
using namespace firefly::input;
Mouse::Mouse() : _captured(false) { _bus->on(this, &Mouse::onEvent); }
void Mouse::onEvent(runtime::Event_SDL &e) {

  auto &event = e.getEvent();
  if (event.type == SDL_MOUSEMOTION) {
    glm::vec2 delta = {event.motion.xrel, event.motion.yrel};
    _position += delta;
    _bus->emit<Event_Mouse>(_position, delta);
    if (_captured) {
      auto win = SDL_GL_GetCurrentWindow();
      int w, h;
      SDL_GetWindowSize(win, &w, &h);
      SDL_WarpMouseInWindow(win, w / 2, h / 2);
    }
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    _bus->emit<Event_MouseButtonDown>(event.button.button);
  }
  if (event.type == SDL_MOUSEWHEEL) {
    glm::vec2 delta = {event.wheel.x, event.wheel.y};
    _wheel += delta;
    _bus->emit<Event_MouseWheel>(_wheel, delta);
  }
}
const glm::vec2 &Mouse::getPosition() const { return _position; }
void Mouse::captureMouse() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  _captured = true;
}
void Mouse::releaseMouse() {
  _captured = false;
  SDL_SetRelativeMouseMode(SDL_FALSE);
}
const bool &Mouse::isCaptured() { return _captured; }