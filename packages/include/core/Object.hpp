#pragma once

#include "AutoPtr.hpp"
#include "core/AutoPtr.hpp"
#include <cstdint>
#include <fmt/core.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace firefly::core {
class Object {
private:
  uint32_t _ref;
  std::string _identity;
  uint32_t _version;

  std::unordered_map<std::string, core::AutoPtr<Object>> _metadata;

public:
  inline const uint32_t &addRef() { return ++_ref; }

  inline const uint32_t &subRef() { return --_ref; }

  inline const uint32_t &ref() const { return _ref; }

  inline const std::string getIdentity() const { return _identity; }

  Object() : _ref(0), _version(0) {
    _identity = fmt::format("[0x{:x}]", (ptrdiff_t)this);
  };

  virtual void initialize() {}
  virtual ~Object(){};
  void setVersion(const uint32_t &version);
  const uint32_t &getVersion() const;

  void setMetadata(const std::string &name, const core::AutoPtr<Object> &data);
  core::AutoPtr<Object> getMetadata(const std::string &name);
  const core::AutoPtr<Object> getMetadata(const std::string &name) const;
};
} // namespace firefly::core