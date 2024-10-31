#pragma once
#include "runtime/Event.hpp"

namespace firefly::input {
class MouseDownEvent : public runtime::Event {
private:
  core::Unsigned_t _type;

public:
  MouseDownEvent(core::Unsigned_t type);
  const core::Unsigned_t &getType() const;
};
}; // namespace firefly::input