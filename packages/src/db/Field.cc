#include "db/Field.hpp"
#include <fmt/core.h>
using namespace firefly::db;
Field::Field(const std::string &name, const std::string &ns, const TYPE &type,
             const bool &isReadonly, const bool &isRequired,
             const bool &isArray)
    : Entity(name, ns), _type(type), _readonly(isReadonly),
      _required(isRequired), _array(isArray) {}
const Field::TYPE &Field::getType() const { return _type; }
const bool &Field::isReadonly() const { return _readonly; }
const bool &Field::isRequired() const { return _required; }
const std::string Field::getTypeName() const {
  switch (_type) {
  case TYPE::STRING:
    return "STRING";
  case TYPE::NUMBER:
    return "NUMBER";
  case TYPE::INTEGER:
    return "INTEGER";
  case TYPE::BOOLEAN:
    return "BOOLEAN";
  case TYPE::O2M:
    return "O2M";
  case TYPE::M2O:
    return "M2O";
  case TYPE::O2O:
    return "O2O";
  case TYPE::M2M:
    return "M2M";
  case TYPE::ENUM:
    return "ENUM";
  }
  return "Unknown";
};
const bool &Field::isArray() const { return _array; }