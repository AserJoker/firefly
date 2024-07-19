#include "db/Field.hpp"
#include "db/Entity.hpp"
#include <stdexcept>
using namespace firefly::db;
Field::Field(const std::string &name, const std::string &ns, const TYPE &type,
             const Attribute attr)
    : Entity(name, ns), _type(type), _attribute(attr) {}
const Field::TYPE &Field::getType() const { return _type; }
const Field::Attribute &Field::getAttribute() const { return _attribute; }

const std::string Field::getTypeName() const {
  switch (_type) {
  case O2M:
    return "O2M";
  case M2O:
    return "M2O";
  case O2O:
    return "O2O";
  case M2M:
    return "M2M";
  case STRING:
    return "STRING";
  case BOOLEAN:
    return "BOOLEAN";
  case FLOAT:
    return "FLOAT";
  case INTEGER:
    return "INTEGER";
  case ENUM:
    return "ENUM";
  }
  throw std::runtime_error("Unknown field type");
}