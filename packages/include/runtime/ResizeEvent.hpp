#pragma once
#include "core/Size.hpp"
#include "runtime/Event.hpp"
#include <SDL_video.h>

namespace firefly::runtime {
class ResizeEvent : public Event {
private:
  core::Size<> _size;
  uint32_t _windowId;

public:
  ResizeEvent(const core::Size<> &size, uint32_t windowId)
      : _size(size), _windowId(windowId) {}
  const core::Size<> &getSize() const { return _size; }
  const uint32_t getWindowId() const { return _windowId; }
};
} // namespace firefly::runtime