#include "database/Metadata.hpp"
using namespace firefly;
using namespace firefly::database;
Metadata::Metadata(const std::string &name, const std::string &ns,
                   const std::vector<Field> &fields,
                   const std::vector<std::string> &primaryKeys,
                   const std::string &driver)
    : _name(name), _namespace(ns), _fields(fields), _primaryKeys(primaryKeys),
      _driver(driver) {}

const std::string &Metadata::getName() const { return _name; }
const std::string &Metadata::getNamespace() const { return _namespace; }
const Field &Metadata::getField(const std::string &name) const {
  return _fields.at(_indices.at(name));
}
const std::vector<Field> &Metadata::getFields() const { return _fields; }
const std::vector<std::string> &Metadata::getPrimaryKeys() const {
  return _primaryKeys;
}

const std::string &Metadata::getDriver() const { return _driver; }