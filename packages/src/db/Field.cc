#include "db/Field.hpp"
#include "db/Entity.hpp"
#include "db/Record.hpp"
#include <fmt/core.h>
#include <stdexcept>
using namespace firefly::db;
static Field::TYPE fromString(const std::string &name) {
  if (name == "O2M") {
    return Field::O2M;
  } else if (name == "M2O") {
    return Field::M2O;
  } else if (name == "O2O") {
    return Field::O2O;
  } else if (name == "M2M") {
    return Field::M2M;
  } else if (name == "STRING") {
    return Field::STRING;
  } else if (name == "BOOLEAN") {
    return Field::BOOLEAN;
  } else if (name == "INTEGER") {
    return Field::INTEGER;
  } else if (name == "FLOAT") {
    return Field::FLOAT;
  } else if (name == "ENUM") {
    return Field::ENUM;
  }
  throw std::runtime_error(fmt::format("Unknown field type '{}'", name));
}
Field::Field(const std::string &name, const std::string &ns, const TYPE &type,
             const Attribute attr)
    : _type(type), _attribute(attr) {
  Entity::initialize(name, ns);
}

Field::Field(const std::string &name, const std::string &ns, const TYPE &type,
             const std::string &refModel,
             const std::vector<std::string> &relationFields,
             const std::vector<std::string> &relatedFields,
             const Attribute attr)
    : Field(name, ns, type, attr) {
  _refModel = refModel;
  _relationFields = relationFields;
  _relatedFields = relationFields;
}
Field::Field(const std::string &name, const std::string &ns, const TYPE &type,
             const std::vector<std::string> &options, const Attribute attr)
    : Field(name, ns, type, attr) {
  _options = options;
}
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
Field::Field() {}
Field::Field(const core::AutoPtr<Record> &record)
    : Field(record->getStringField("name"), record->getStringField("namespace"),
            fromString(record->getEnumField("type")),
            {record->getBooleanField(record->getStringField("required")),
             record->getBooleanField("readonly"),
             record->getBooleanField("array")}) {
  if (_type <= M2M) {
    _refModel = record->getStringField("refModel");
    _relatedFields = record->getStringArrayField("relatedFields");
    _relationFields = record->getStringArrayField("relationFields");
  }
  if (_type == ENUM) {
    _options = record->getStringArrayField("options");
  }
}
const std::string &Field::getRefModel() const { return _refModel; }
const std::vector<std::string> &Field::getRelationFields() const {
  return _relationFields;
}
const std::vector<std::string> &Field::getRelatedFields() const {
  return _relatedFields;
}
const std::vector<std::string> &Field::getOptions() const { return _options; }