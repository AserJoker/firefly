#pragma once
#include "Factory.hpp"
#include "ObjectBase.hpp"
#include <fmt/format.h>
namespace firefly::core {
class Provider {
public:
  template <class T, class K = T> static void provide() {
    Factory::provide(typeid(T).name(),
                     []() -> ObjectBase * { return new K(); });
  }
};
} // namespace firefly::core