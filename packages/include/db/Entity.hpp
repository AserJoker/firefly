#pragma once
#include "core/Object.hpp"
#include <fmt/core.h>
#include <string>
namespace firefly::db {
class Entity : public core::Object {
private:
  std::string _name;
  std::string _namespace;
  std::string _id;

public:
  Entity();
  void initialize(const std::string &name, const std::string &ns,
                  const std::string &id = "");
  const std::string &getName() const;
  const std::string &getNamespace() const;
  const std::string &getId() const;
};
}; // namespace firefly::db