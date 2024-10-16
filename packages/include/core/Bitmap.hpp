#pragma once
#include "core/Object.hpp"
#include <string>
#include "core/Map.hpp"
namespace firefly::core {
class Bitmap : public Object {
private:
  core::Map<std::string, bool> _bitmap;

public:
  const bool check(const std::string &name) const;
  void enable(const std::string &name);
  void disable(const std::string &name);
  void clear();
  const core::Map<std::string, bool> &getData() const;
};
} // namespace firefly::core