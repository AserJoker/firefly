#pragma once
#include "runtime/Event.hpp"
#include <cstdint>
namespace firefly::input {
class Event_KeyDown : public runtime::Event {
private:
  uint32_t _scancode;

public:
  Event_KeyDown(const uint32_t &scancode);
  const uint32_t &getScancode() const;
};
}; // namespace firefly::input