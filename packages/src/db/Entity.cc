#include "db/Entity.hpp"
#include <fmt/core.h>
using namespace firefly::db;
void Entity::initialize(const std::string &name, const std::string &ns,
                        const std::string &id) {
  _name = name;
  _namespace = ns;
  _id = id.empty() ? fmt::format("{}.{}", _namespace, _name) : id;
}
const std::string &Entity::getName() const { return _name; }
const std::string &Entity::getNamespace() const { return _namespace; }
const std::string &Entity::getId() const { return _id; }