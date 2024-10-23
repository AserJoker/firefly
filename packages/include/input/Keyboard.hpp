#pragma once
#include "Component.hpp"
#include "core/Injector.hpp"
#include "core/Object.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/SystemEvent.hpp"
namespace firefly::input {
class Keyboard : public core::Object {
private:
  bool _bitmap[512];
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _bus;

private:
  void onEvent(runtime::SystemEvent &e);

public:
  Keyboard();
  const bool &getKeyState(uint32_t scancode) const;
};
}; // namespace firefly::input