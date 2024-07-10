#pragma once
#include "core/Object.hpp"
#include "runtime/Event_SDL.hpp"
namespace firefly::input {
class Keyboard : public core::Object {
private:
  bool _bitmap[512];

private:
  void onEvent(runtime::Event_SDL &e);

public:
  Keyboard();
  const bool &getKeyState(const uint32_t &scancode) const;
};
}; // namespace firefly::input