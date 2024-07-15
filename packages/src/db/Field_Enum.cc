#include "db/Field_Enum.hpp"
#include "db/Field.hpp"
using namespace firefly::db;
Field_Enum::Field_Enum(const std::string &name, const std::string &ns,
                       const std::vector<std::string> &enums,
                       const bool &isReadonly, const bool &isRequired,
                       const bool &isArray)
    : Field(name, ns, Field::TYPE::ENUM, isReadonly, isRequired, isArray),
      _enums(enums) {}
const std::vector<std::string> &Field_Enum::getEnums() const { return _enums; }