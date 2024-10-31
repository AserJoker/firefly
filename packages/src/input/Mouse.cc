#include "input/Mouse.hpp"
#include "input/ClickEvent.hpp"
#include "input/MouseDownEvent.hpp"
#include "input/MouseMotionEvent.hpp"
#include "input/MouseUpEvent.hpp"
#include "input/MouseWheelEvent.hpp"
#include "runtime/SystemEvent.hpp"
#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_video.h>
#include <glm/fwd.hpp>

using namespace firefly;
using namespace firefly::input;
Mouse::Mouse() : _captured(false) { _bus->on(this, &Mouse::onEvent); }

void Mouse::onEvent(runtime::SystemEvent &e) {
  auto &event = e.getEvent();
  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 delta = {event.motion.xrel, event.motion.yrel};
    glm::ivec2 position;
    SDL_GetMouseState(&position.x, &position.y);
    _bus->emit<MouseMotionEvent>(position, delta);
    if (_captured) {
      auto win = SDL_GL_GetCurrentWindow();
      int w, h;
      SDL_GetWindowSize(win, &w, &h);
      SDL_WarpMouseInWindow(win, w / 2, h / 2);
    }
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    _bus->emit<MouseDownEvent>(event.button.button);
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    _bus->emit<MouseUpEvent>(event.button.button);
    _bus->emit<ClickEvent>(event.button.button);
  }
  if (event.type == SDL_MOUSEWHEEL) {
    glm::vec2 delta = {event.wheel.x, event.wheel.y};
    _wheel += delta;
    _bus->emit<MouseWheelEvent>(_wheel, delta);
  }
}

const glm::ivec2 Mouse::getPosition() const {
  glm::ivec2 position;
  SDL_GetMouseState(&position.x, &position.y);
  return position;
}

void Mouse::captureMouse() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  _captured = true;
}

void Mouse::releaseMouse() {
  auto win = SDL_GL_GetCurrentWindow();
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_WarpMouseInWindow(win, w / 2, h / 2);
  _captured = false;
  SDL_SetRelativeMouseMode(SDL_FALSE);
}

const core::Boolean_t &Mouse::isCaptured() { return _captured; }