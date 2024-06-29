#pragma once
#include "core/AutoPtr.hpp"
namespace firefly::core {
template <class T> class Singleton {
private:
  inline static AutoPtr<T> _instance;

public:
  static AutoPtr<T> &instance() { return _instance; }
};
} // namespace firefly::core