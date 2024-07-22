#pragma once
#include "Entity.hpp"
#include "core/AutoPtr.hpp"
#include <vector>
namespace firefly::db {
class Record;
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
  std::string _refModel;
  std::vector<std::string> _relationFields;
  std::vector<std::string> _relatedFields;
  std::vector<std::string> _options;

public:
  Field();
  Field(const std::string &name, const std::string &ns, const TYPE &type,
        const Attribute attr = {});
  Field(const std::string &name, const std::string &ns, const TYPE &type,
        const std::string &refModel,
        const std::vector<std::string> &relationFields,
        const std::vector<std::string> &relatedFields,
        const Attribute attr = {});
  Field(const std::string &name, const std::string &ns, const TYPE &type,
        const std::vector<std::string> &options, const Attribute attr = {});
  Field(const core::AutoPtr<Record> &record);
  const TYPE &getType() const;
  const std::string getTypeName() const;
  const Attribute &getAttribute() const;
  const std::string &getRefModel() const;
  const std::vector<std::string> &getRelationFields() const;
  const std::vector<std::string> &getRelatedFields() const;
  const std::vector<std::string> &getOptions() const;
};
}; // namespace firefly::db