#pragma once
#include "core/Rect.hpp"
#include "runtime/Event.hpp"

namespace firefly::runtime {
class Event_Resize : public Event {
private:
  core::Size<> _size;

public:
  Event_Resize(const core::Size<> &size) : _size(size) {}
  const core::Size<> &getSize() const { return _size; }
};
} // namespace firefly::runtime