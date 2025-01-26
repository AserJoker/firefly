#pragma once
#include "AutoPtr.hpp"
#include "ObjectBase.hpp"
#include <string>
#include <unordered_map>
namespace firefly::core {
class Registry {
private:
  static inline std::unordered_map<std::string, core::AutoPtr<ObjectBase>>
      _registrys;

public:
  static void store(const std::string &name,
                    const core::AutoPtr<ObjectBase> &object);
  static core::AutoPtr<ObjectBase> query(const std::string &name);
  static bool contains(const std::string &name);
};
} // namespace firefly::core