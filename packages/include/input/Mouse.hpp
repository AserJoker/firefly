#pragma once
#include "Component.hpp"
#include "core/Injector.hpp"
#include "core/Object.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/SystemEvent.hpp"
#include <glm/glm.hpp>
namespace firefly::input {
class Mouse : public core::Object {
private:
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _bus;
  core::Boolean_t _captured;
  glm::ivec2 _wheel;

private:
  void onEvent(runtime::SystemEvent &e);

public:
  Mouse();
  const glm::ivec2 getPosition() const;
  void captureMouse();
  void releaseMouse();
  const core::Boolean_t &isCaptured();
};
}; // namespace firefly::input