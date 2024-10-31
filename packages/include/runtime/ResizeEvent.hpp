#pragma once
#include "core/Size.hpp"
#include "runtime/Event.hpp"
#include <SDL_video.h>

namespace firefly::runtime {
class ResizeEvent : public Event {
private:
  core::Size<> _size;
  core::Unsigned_t _windowId;

public:
  ResizeEvent(const core::Size<> &size, core::Unsigned_t windowId)
      : _size(size), _windowId(windowId) {}
  const core::Size<> &getSize() const { return _size; }
  const core::Unsigned_t getWindowId() const { return _windowId; }
};
} // namespace firefly::runtime