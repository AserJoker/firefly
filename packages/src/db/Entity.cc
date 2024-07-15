#include "db/Entity.hpp"
#include <fmt/core.h>
using namespace firefly::db;
Entity::Entity(const std::string &name, const std::string &ns,
               const std::string &id)
    : _id(id.empty() ? fmt::format("{}.{}", ns, name) : id), _name(name),
      _namespace(ns) {}
const std::string &Entity::getId() const { return _id; }
const std::string &Entity::getName() const { return _name; }
const std::string &Entity::getNamespace() const { return _namespace; }