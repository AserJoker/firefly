#pragma once
#include "Field.hpp"
#include <vector>
namespace firefly::db {
class Field_Enum : public Field {
private:
  std::vector<std::string> _enums;

public:
  Field_Enum(const std::string &name, const std::string &ns,
             const std::vector<std::string> &enums,
             const bool &isReadonly = false, const bool &isRequired = false,
             const bool &isArray = false);
  const std::vector<std::string> &getEnums() const;
};
} // namespace firefly::db