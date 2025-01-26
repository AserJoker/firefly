#pragma once
#include "AutoPtr.hpp"
#include "Event.hpp"
#include "EventBus.hpp"
#include "ObjectBase.hpp"
#include "core/Singleton.hpp"
#include <cassert>
#include <functional>
#include <unordered_map>

namespace firefly::core {
class Object : public ObjectBase {
private:
  AutoPtr<EventBus> _bus = Singleton<EventBus>::instance();
  std::unordered_map<std::string,
                     std::vector<std::function<void(const Event &event)>>>
      _listeners;

protected:
  static inline std::function<void()> _static_block = []() {};

public:
  Object();

  void emit(const Event &event);

  void on(const Event &event);

  template <class E = Event> void emit(auto... args) {
    E event{args...};
    if (event.getType().empty()) {
      event.setType(typeid(E).name());
    }
    emit(event);
  }

  template <class T, class E> void addListener(void (T::*callback)(const E &)) {
    if (!_listeners.contains(typeid(E).name())) {
      _bus->on(typeid(E).name(), getHandle());
    }
    auto &listeners = _listeners[typeid(E).name()];
    listeners.push_back([=, this](const Event &event) {
      (((T *)this)->*callback)((const E &)event);
    });
  }

  template <class T>
  void addListener(const std::string &event, void (T::*callback)()) {
    if (!_listeners.contains(event)) {
      _bus->on(event, getHandle());
    }
    auto &listeners = _listeners[event];
    auto fn = std::function{
        [=, this](const Event &event) { (((T *)this)->*callback)(); }};
    listeners.push_back(fn);
  }
};
} // namespace firefly::core