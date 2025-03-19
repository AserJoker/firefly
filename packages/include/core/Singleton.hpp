#pragma once
#include "AutoPtr.hpp"
#include "Factory.hpp"
#include "Registry.hpp"
#include <type_traits>
namespace firefly::core {

template <class T> class Singleton {
public:
  static AutoPtr<T> instance()
    requires(std::is_default_constructible_v<T>)
  {
    auto name = typeid(T).name();
    if (!Registry::contains(name)) {
      auto obj = Factory::create(name);
      if (!obj) {
        obj = new T();
      }
      Registry::store(name, obj);
    }
    return Registry::query(name).cast<T>();
  }
  static AutoPtr<T> instance() {
    auto name = typeid(T).name();
    if (!Registry::contains(name)) {
      auto obj = Factory::create(name);
      if (!obj) {
        return nullptr;
      }
      Registry::store(name, obj);
    }
    return Registry::query(name).cast<T>();
  }
};
} // namespace firefly::core