#include "db/Field_Complex.hpp"
using namespace firefly::db;
Field_Complex::Field_Complex(const std::string &name, const std::string &ns,
                             const COMPLEX_TYPE &type,
                             std::string relationTable,
                             std::vector<std::string> relationFields,
                             std::vector<std::string> relatedFields,
                             const bool &isReadonly, const bool &isRequired)
    : Field(name, ns, (Field::TYPE)type, isReadonly, isRequired, false),
      _relationTable(relationTable), _relationFields(relationFields),
      _relatedFields(relatedFields) {}
const std::string &Field_Complex::getRelationTable() const {
  return _relationTable;
}
const std::vector<std::string> &Field_Complex::getRelationFields() const {
  return _relationFields;
}
const std::vector<std::string> &Field_Complex::getRelatedFields() const {
  return _relatedFields;
}