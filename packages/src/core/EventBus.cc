#include "core/EventBus.hpp"
#include "core/Object.hpp"
using namespace firefly;
using namespace firefly::core;

void EventBus::emit(const HANDLE &emitter, const Event &event) {
  auto &listeners = _listeners[event.getType()];
  for (auto &handle : listeners) {
    auto obj = (Object *)Object::select(handle);
    if (obj) {
      obj->on(event);
    }
  }
}

void EventBus::on(const std::string &event, const HANDLE &listener) {
  auto &listeners = _listeners[event];
  listeners.insert(listener);
}

void EventBus::off(const std::string &event, const HANDLE &listener) {
  auto &listeners = _listeners[event];
  listeners.erase(listener);
}