#pragma once
#include "runtime/Event.hpp"
#include <glm/glm.hpp>
namespace firefly::input {
class Event_Mouse : public runtime::Event {
private:
  glm::vec2 _position;
  glm::vec2 _delta;

public:
  Event_Mouse(const glm::vec2 &position, const glm::vec2 &delta);
  const glm::vec2 &getPosition() const;
  const glm::vec2 &getDelta() const;
};
}; // namespace firefly::input