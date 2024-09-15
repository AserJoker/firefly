#pragma once
#include "runtime/Event.hpp"
#include <cstdint>
namespace firefly::input {
class Event_Click : public runtime::Event {
private:
  uint32_t _type;

public:
  Event_Click(uint32_t type);
  const uint32_t &getType() const;
};
}; // namespace firefly::input