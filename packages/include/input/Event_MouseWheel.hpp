#pragma once
#include "runtime/Event.hpp"
#include <glm/glm.hpp>
namespace firefly::input {
class Event_MouseWheel : public runtime::Event {
private:
  glm::vec2 _position;
  glm::vec2 _delta;

public:
  Event_MouseWheel(const glm::vec2 &offset, const glm::vec2 &delta);
  const glm::vec2 &getOffset() const;
  const glm::vec2 &getDelta() const;
};
} // namespace firefly::input