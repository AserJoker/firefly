#pragma once
#include "Component.hpp"
#include "core/Injector.hpp"
#include "core/Object.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_SDL.hpp"
#include <glm/glm.hpp>
namespace firefly::input {
class Mouse : public core::Object {
private:
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _bus;
  glm::vec2 _position;
  bool _captured;
  glm::vec2 _wheel;

private:
  void onEvent(runtime::Event_SDL &e);

public:
  Mouse();
  const glm::vec2 &getPosition() const;
  void captureMouse();
  void releaseMouse();
  const bool &isCaptured();
};
}; // namespace firefly::input