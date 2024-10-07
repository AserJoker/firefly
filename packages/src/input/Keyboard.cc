#include "input/Keyboard.hpp"
#include "core/Singleton.hpp"
#include "input/Event_KeyUp.hpp"
#include "input/Event_Keydown.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_SDL.hpp"
#include <cstring>
using namespace firefly;
using namespace firefly::input;
Keyboard::Keyboard() {
  memset(_bitmap, 0, sizeof(_bitmap));
  auto bus = core::Singleton<runtime::EventBus>::instance();
  bus->on(this, &Keyboard::onEvent);
}
void Keyboard::onEvent(runtime::Event_SDL &e) {
  auto &event = e.getEvent();
  auto bus = core::Singleton<runtime::EventBus>::instance();
  switch (event.type) {
  case SDL_KEYDOWN:
    if (!_bitmap[event.key.keysym.scancode]) {
      _bitmap[event.key.keysym.scancode] = true;
      bus->emit<Event_KeyDown>(event.key.keysym.scancode);
    }
    break;
  case SDL_KEYUP:
    if (_bitmap[event.key.keysym.scancode]) {
      _bitmap[event.key.keysym.scancode] = false;
      bus->emit<Event_KeyUp>(event.key.keysym.scancode);
    }
    break;
  }
}
const bool &Keyboard::getKeyState(uint32_t scancode) const {
  return _bitmap[scancode];
}