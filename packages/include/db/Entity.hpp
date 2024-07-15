#pragma once
#include "core/Object.hpp"
#include <fmt/core.h>
#include <string>
namespace firefly::db {
class Entity : public core::Object {
private:
  std::string _id;
  std::string _name;
  std::string _namespace;

public:
  Entity(const std::string &name, const std::string &ns,
         const std::string &id = "");
  const std::string &getId() const;
  const std::string &getName() const;
  const std::string &getNamespace() const;
};
} // namespace firefly::db