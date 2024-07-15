#pragma once
#include "Field.hpp"
#include <vector>
namespace firefly::db {
class Field_Complex : public Field {
private:
  std::string _relationTable;
  std::vector<std::string> _relationFields;
  std::vector<std::string> _relatedFields;

public:
  Field_Complex(const std::string &name, const std::string &ns,
                const COMPLEX_TYPE &type, std::string relationTable,
                std::vector<std::string> relationFields,
                std::vector<std::string> referenceFields,
                const bool &isReadonly = false, const bool &isRequired = false);
  const std::string &getRelationTable() const;
  const std::vector<std::string> &getRelationFields() const;
  const std::vector<std::string> &getRelatedFields() const;
};
} // namespace firefly::db