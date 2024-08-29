#pragma once
#include "core/Object.hpp"
namespace firefly::video {
class Material : public core::Object {
private:
  std::string _type;

public:
  Material(const std::string &type);
  const std::string &getType() const;
};
} // namespace firefly::video