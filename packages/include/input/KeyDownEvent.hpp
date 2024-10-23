#pragma once
#include "runtime/Event.hpp"
#include <cstdint>
namespace firefly::input {
class KeyDownEvent : public runtime::Event {
private:
  uint32_t _scancode;

public:
  KeyDownEvent(uint32_t scancode);
  const uint32_t &getScancode() const;
};
}; // namespace firefly::input