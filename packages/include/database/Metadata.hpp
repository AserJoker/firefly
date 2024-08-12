#pragma once
#include "Field.hpp"
#include "core/Object.hpp"
#include <unordered_map>
#include <vector>
namespace firefly::database {
class Metadata : public core::Object {
private:
  std::vector<Field> _fields;
  std::unordered_map<std::string, uint32_t> _indices;
  std::vector<std::string> _primaryKeys;
  std::string _name;
  std::string _namespace;
  std::string _driver;

public:
  Metadata(const std::string &name, const std::string &ns,
           const std::vector<Field> &fields,
           const std::vector<std::string> &primaryKeys = {"id"},
           const std::string &driver = "");
  const std::string &getName() const;
  const std::string &getNamespace() const;
  const Field &getField(const std::string &name) const;
  const std::vector<Field> &getFields() const;
  const std::vector<std::string> &getPrimaryKeys() const;
  const std::string &getDriver() const;
};
} // namespace firefly::database