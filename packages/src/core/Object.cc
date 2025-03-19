#include "core/Object.hpp"
#include "core/Event.hpp"
using namespace firefly;
using namespace firefly::core;
Object::Object() {}

void Object::on(const Event &event) {
  if (_listeners.contains(event.getType())) {
    auto &listeners = _listeners.at(event.getType());
    for (auto &listener : listeners) {
      listener(event);
    }
  }
}

void Object::emit(const Event &event) { _bus->emit(getHandle(), event); }
