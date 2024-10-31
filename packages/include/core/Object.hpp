#pragma once

#include "Type.hpp"
#include <fmt/core.h>

namespace firefly::core {
class Object {
private:
  core::Unsigned_t _ref;
  core::String_t _identity;

public:
  inline const core::Unsigned_t &addRef() { return ++_ref; }

  inline const core::Unsigned_t &subRef() { return --_ref; }

  inline const core::Unsigned_t &ref() const { return _ref; }

  inline const core::String_t getIdentity() const { return _identity; }

  Object() : _ref(0) { _identity = fmt::format("[0x{:x}]", (ptrdiff_t)this); };

  virtual ~Object() = default;
};
} // namespace firefly::core