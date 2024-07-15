#pragma once
#include "Entity.hpp"
#include <string>
namespace firefly::db {
class Field : public Entity {
public:
  enum class COMPLEX_TYPE { O2M, M2O, O2O, M2M };
  enum class TYPE {
    O2M = (int)COMPLEX_TYPE::O2M,
    M2O = (int)COMPLEX_TYPE::M2O,
    O2O = (int)COMPLEX_TYPE::O2O,
    M2M = (int)COMPLEX_TYPE::M2M,
    STRING,
    INTEGER,
    NUMBER,
    BOOLEAN,
    ENUM
  };

private:
  TYPE _type;
  bool _readonly;
  bool _required;
  bool _array;

public:
  Field(const std::string &name, const std::string &ns, const TYPE &type,
        const bool &isReadonly = false, const bool &isRequired = false,
        const bool &isArray = false);
  const TYPE &getType() const;
  const bool &isReadonly() const;
  const bool &isRequired() const;
  const bool &isArray() const;
  const std::string getTypeName() const;
};
}; // namespace firefly::db