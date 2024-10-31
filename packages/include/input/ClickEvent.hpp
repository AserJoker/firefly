#pragma once
#include "runtime/Event.hpp"

namespace firefly::input {
class ClickEvent : public runtime::Event {
private:
  core::Unsigned_t _type;

public:
  ClickEvent(core::Unsigned_t type);
  const core::Unsigned_t &getType() const;
};
}; // namespace firefly::input