#pragma once
#include "Entity.hpp"
namespace firefly::db {
class Field : public Entity {
public:
  enum TYPE { O2M, M2O, O2O, M2M, STRING, BOOLEAN, FLOAT, INTEGER, ENUM };
  struct Attribute {
    bool required;
    bool readonly;
    bool array;
  };

private:
  TYPE _type;
  Attribute _attribute;

public:
  Field(const std::string &name, const std::string &ns, const TYPE &type,
        const Attribute attr = {});
  const TYPE &getType() const;
  const std::string getTypeName() const;
  const Attribute &getAttribute() const;
};
}; // namespace firefly::db