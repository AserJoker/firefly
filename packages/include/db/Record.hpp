#pragma once
#include "Field.hpp"
#include "core/Object.hpp"
#include <any>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::db {
class Record : public core::Object {
private:
  std::unordered_map<std::string, std::any> _record;
  std::unordered_map<std::string, Field> _fields;
  std::vector<std::string> _primaryKeys;

public:
  const std::unordered_map<std::string, std::any> &getData() const;
  Record(const std::vector<Field> &fields,
         const std::unordered_map<std::string, std::any> &data = {},
         const std::vector<std::string> &primaryKeys = {"id"});
  const std::string getKey() const;
  const bool
  match(const std::unordered_map<std::string, std::any> &filter) const;
  const bool hasField(const std::string &name) const;
  const std::any &getField(const std::string &name) const;
  void setField(const std::string &name, const std::any &value);

  const std::string getStringField(const std::string &name) const;
  const std::vector<std::string>
  getStringArrayField(const std::string &name) const;
  const int32_t getIntegerField(const std::string &name) const;
  const std::vector<int32_t>
  getIntegerArrayField(const std::string &name) const;
  const float_t getFloatField(const std::string &name) const;
  const std::vector<float_t> getFloatArrayField(const std::string &name) const;
  const std::string getEnumField(const std::string &name) const;
  const std::vector<std::string>
  getEnumArrayField(const std::string &name) const;
};
} // namespace firefly::db