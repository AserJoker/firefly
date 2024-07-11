#pragma once
#include "core/Object.hpp"
#include "runtime/Event_SDL.hpp"
#include <glm/glm.hpp>
namespace firefly::input {
class Mouse : public core::Object {
private:
  glm::vec2 _position;
  bool _catched;

private:
  void onEvent(runtime::Event_SDL &e);

public:
  Mouse();
  const glm::vec2 &getPosition() const;
  void catchMouse();
  void releaseMouse();
  const bool& isCatched();
};
}; // namespace firefly::input