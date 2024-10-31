#pragma once
#include "runtime/Event.hpp"

namespace firefly::input {
class KeyDownEvent : public runtime::Event {
private:
  core::Unsigned_t _scancode;

public:
  KeyDownEvent(core::Unsigned_t scancode);
  const core::Unsigned_t &getScancode() const;
};
}; // namespace firefly::input