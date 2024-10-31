#include "input/Keyboard.hpp"
#include "input/KeyDownEvent.hpp"
#include "input/KeyUpEvent.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/SystemEvent.hpp"
#include <cstring>

using namespace firefly;
using namespace firefly::input;
Keyboard::Keyboard() {
  memset(_bitmap, 0, sizeof(_bitmap));
  _bus->on(this, &Keyboard::onEvent);
}
void Keyboard::onEvent(runtime::SystemEvent &e) {
  auto &event = e.getEvent();

  switch (event.type) {
  case SDL_KEYDOWN:
    if (!_bitmap[event.key.keysym.scancode]) {
      _bitmap[event.key.keysym.scancode] = true;
      _bus->emit<KeyDownEvent>(event.key.keysym.scancode);
    }
    break;
  case SDL_KEYUP:
    if (_bitmap[event.key.keysym.scancode]) {
      _bitmap[event.key.keysym.scancode] = false;
      _bus->emit<KeyUpEvent>(event.key.keysym.scancode);
    }
    break;
  }
}
const core::Boolean_t &Keyboard::getKeyState(core::Unsigned_t scancode) const {
  return _bitmap[scancode];
}