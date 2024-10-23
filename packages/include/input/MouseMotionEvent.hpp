#pragma once
#include "runtime/Event.hpp"
#include <glm/glm.hpp>
namespace firefly::input {
class MouseMotionEvent : public runtime::Event {
private:
  glm::ivec2 _position;
  glm::ivec2 _delta;

public:
  MouseMotionEvent(const glm::ivec2 &position, const glm::ivec2 &delta);
  const glm::ivec2 &getPosition() const;
  const glm::ivec2 &getDelta() const;
};
}; // namespace firefly::input