#pragma once

#include "core/AutoPtr.hpp"

namespace firefly::core {
template <class T> class Singleton {
private:
  inline static AutoPtr<T> _instance;

public:
  static AutoPtr<T> &instance()
    requires std::is_default_constructible_v<T>
  {
    if (!_instance) {
      _instance = new T();
    }
    return _instance;
  }

  template <class K = T, class... ARGS>
  static void initialize(ARGS... args)
    requires std::is_constructible_v<K, ARGS...> &&
             std::is_convertible_v<K *, T *>
  {
    _instance = new K(args...);
  }

  static AutoPtr<T> &instance() { return _instance; }

  static bool isInitialized() { return _instance != nullptr; }
};
} // namespace firefly::core