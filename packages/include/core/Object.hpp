#pragma once

#include <cstdint>

namespace firefly::core {
class Object {
private:
  uint32_t _ref;

public:
  inline const uint32_t &addRef() { return ++_ref; }

  inline const uint32_t &subRef() { return --_ref; }

  inline const uint32_t &ref() const { return _ref; }

  Object() : _ref(0){};

  virtual void initialize() {}

  virtual ~Object() = default;
};
} // namespace firefly::core