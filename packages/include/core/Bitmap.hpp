#pragma once
#include "core/Object.hpp"
#include <unordered_map>
namespace firefly::core {
class Bitmap : public Object {
private:
  std::unordered_map<std::string, bool> _bitmap;

public:
  const bool check(const std::string &name) const;
  void enable(const std::string &name);
  void disable(const std::string &name);
  void clear();
};
} // namespace firefly::core