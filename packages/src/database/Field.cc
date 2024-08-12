#include "database/Field.hpp"
using namespace firefly;
using namespace firefly::database;

Field::Field(const TYPE &type, const std::string &name, const std::string &ns,
             bool required, bool readonly, bool array)
    : _type(type), _name(name), _namespace(ns), _required(required),
      _readonly(readonly), _array(array) {}
const Field::TYPE &Field::getType() const { return _type; }
const std::string &Field::getName() const { return _name; }
const std::string &Field::getNamespace() const { return _namespace; }
const bool &Field::isRequired() const { return _required; }
const bool &Field::isReadonly() const { return _readonly; }
const bool &Field::isArray() const { return _array; }