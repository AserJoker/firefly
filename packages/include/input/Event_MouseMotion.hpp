#pragma once
#include "runtime/Event.hpp"
#include <glm/glm.hpp>
namespace firefly::input {
class Event_MouseMotion : public runtime::Event {
private:
  glm::ivec2 _position;
  glm::ivec2 _delta;

public:
  Event_MouseMotion(const glm::ivec2 &position, const glm::ivec2 &delta);
  const glm::ivec2 &getPosition() const;
  const glm::ivec2 &getDelta() const;
};
}; // namespace firefly::input