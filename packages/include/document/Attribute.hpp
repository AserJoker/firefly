#pragma once
#include "core/AnyPtr.hpp"
#include "core/Value.hpp"
namespace firefly::document {
class Attribute : public core::AnyPtr {
public:
  Attribute() = default;
  Attribute(core::String_t *ptr) : core::AnyPtr(ptr){};
  Attribute(core::Integer_t *ptr) : core::AnyPtr(ptr){};
  Attribute(core::Unsigned_t *ptr) : core::AnyPtr(ptr){};
  Attribute(core::Float_t *ptr) : core::AnyPtr(ptr){};
  Attribute(core::Boolean_t *ptr) : core::AnyPtr(ptr){};

  Attribute(const Attribute &attr) = default;
  Attribute &operator=(const Attribute &another) = default;
};
}; // namespace firefly::document