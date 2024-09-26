#pragma once

#include <cstdint>
#include <fmt/core.h>
#include <string>

namespace firefly::core {
class Object {
private:
  uint32_t _ref;
  std::string _identity;

public:
  inline const uint32_t &addRef() { return ++_ref; }

  inline const uint32_t &subRef() { return --_ref; }

  inline const uint32_t &ref() const { return _ref; }

  inline const std::string getIdentity() const { return _identity; }

  Object() : _ref(0) {
    _identity = fmt::format("[0x{:x}]", (ptrdiff_t)this);
  };

  virtual void initialize() {}
  virtual ~Object() = default;

};
} // namespace firefly::core