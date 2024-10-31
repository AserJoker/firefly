#pragma once
#include "runtime/Event.hpp"
namespace firefly::input {
class KeyUpEvent : public runtime::Event {
private:
  core::Unsigned_t _scancode;

public:
  KeyUpEvent(core::Unsigned_t scancode);
  const core::Unsigned_t &getScancode() const;
};
}; // namespace firefly::input