#pragma once

#include <cstdint>
#include <fmt/core.h>
#include <functional>
#include <unordered_map>
#include <vector>

namespace firefly::core {
class Object {
private:
  uint32_t _ref;
  std::string _identity;

  std::vector<std::function<void(const std::string &identity)>> _defers;

private:
  static std::unordered_map<std::string, Object *> _objects;

public:
  inline const uint32_t &addRef() { return ++_ref; }

  inline const uint32_t &subRef() { return --_ref; }

  inline const uint32_t &ref() const { return _ref; }

  inline const std::string getIdentity() const { return _identity; }

  Object() : _ref(0) {
    _identity = fmt::format("[0x{:x}]", (ptrdiff_t)this);
    Object::_objects[_identity] = this;
  };

  virtual void initialize() {}

  void defer(const std::function<void(const std::string &identity)> &callback) {
    _defers.push_back(callback);
  }

  virtual ~Object() {
    Object::_objects.erase(_identity);
    for (auto &cb : _defers) {
      cb(_identity);
    }
  };

public:
  static Object *select(const std::string &id);
};
} // namespace firefly::core