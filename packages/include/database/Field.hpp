#pragma once
#include <string>
namespace firefly::database {
class Field {
public:
  enum class TYPE { STRING, INTEGER, NUMBER, BOOLEAN };

private:
  TYPE _type;
  std::string _name;
  std::string _namespace;
  bool _required;
  bool _readonly;
  bool _array;

public:
  Field(const TYPE &type, const std::string &name, const std::string &ns,
        bool required = false, bool readonly = false, bool array = false);
  const TYPE &getType() const;
  const std::string &getName() const;
  const std::string &getNamespace() const;
  const bool &isRequired() const;
  const bool &isReadonly() const;
  const bool &isArray() const;
};
} // namespace firefly::database