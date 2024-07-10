#include "input/Mouse.hpp"
#include "core/Singleton.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_SDL.hpp"
#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_video.h>
#include <iostream>
using namespace firefly;
using namespace firefly::input;
Mouse::Mouse() : _catched(false) {
  auto bus = core::Singleton<runtime::EventBus>::instance();
  bus->on(this, &Mouse::onEvent);
}
void Mouse::onEvent(runtime::Event_SDL &e) {

  auto bus = core::Singleton<runtime::EventBus>::instance();
  auto &event = e.getEvent();
  if (_catched) {
    if (event.type == SDL_MOUSEMOTION) {
      std::cout << event.motion.x << "," << event.motion.y << ","
                << event.motion.xrel << "," << event.motion.yrel << std::endl;
      glm::vec2 delta = {event.motion.xrel, event.motion.yrel};
      _position += delta;
      bus->emit<Event_Mouse>(_position, delta);
      auto win = SDL_GL_GetCurrentWindow();
      SDL_WarpMouseInWindow(win, 512, 384);
    }
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    bus->emit<Event_MouseButtonDown>(event.button.button);
  }
}
const glm::vec2 &Mouse::getPosition() const { return _position; }
void Mouse::catchMouse() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  _catched = true;
}
void Mouse::releaseMouse() {
  _catched = false;
  SDL_SetRelativeMouseMode(SDL_FALSE);
}