#pragma once
#include "core/Object.hpp"
#include <string>
namespace firefly::db {
class Field : public core::Object {
public:
  enum class TYPE { STRING, NUMBER, BOOLEAN, O2M, M2O, O2O, M2M, ENUM };

private:
  std::string _name;
  TYPE _type;
  bool _readonly;
  bool _required;

public:
  Field(const std::string &name, const TYPE &type, const bool &isReadonly,
        const bool &isRequired);
  const std::string &getName() const;
  const TYPE &getType() const;
  const bool &isReadonly() const;
  const bool &isRequired() const;
  const std::string getTypeName() const;
};
}; // namespace firefly::db