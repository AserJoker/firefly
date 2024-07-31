#pragma once

#include <cstdint>
#include <fmt/core.h>
#include <string>
#ifdef _MSVC_LANG
#include <vcruntime_typeinfo.h>
#else
#include <typeinfo>
#endif

namespace firefly::core {
class Object {
private:
  uint32_t _ref;

public:
  inline const uint32_t &addRef() { return ++_ref; }

  inline const uint32_t &subRef() { return --_ref; }

  inline const uint32_t &ref() const { return _ref; }

  inline const std::string getIdentity() const {
    return fmt::format("[{} 0x{:x}]", typeid(*this).name(), (ptrdiff_t)this);
  }

  Object() : _ref(0){};

  virtual void initialize() {}

  virtual ~Object() = default;
};
} // namespace firefly::core