#pragma once
#include "runtime/Event.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace firefly::runtime {
class Event_Resize : public Event {
private:
  glm::ivec2 _size;

public:
  Event_Resize(const glm::ivec2 &size) : _size(size) {}
  const glm::ivec2 &getSize() const { return _size; }
};
} // namespace firefly::runtime