#pragma once
#include "Field.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <any>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::db {
class Record : public core::Object {
private:
  std::vector<core::AutoPtr<Field>> _fields;
  std::unordered_map<std::string, std::any> _record;

public:
  Record(const std::vector<core::AutoPtr<Field>> &fields,
         const std::unordered_map<std::string, std::any> &record = {});
  const std::string getStringField(const std::string &name) const;
  const std::string getEnumField(const std::string &name) const;
  const int32_t getIntegerField(const std::string &name) const;
  const double getNumberField(const std::string &name) const;
  const bool getBooleanField(const std::string &name) const;
  bool isNil(const std::string &name) const;
  const std::vector<std::string>
  getStringArrayField(const std::string &name) const;
  const std::vector<std::string>
  getEnumArrayField(const std::string &name) const;
  const std::vector<int32_t> getIntegerArrayField(const std::string &name) const;
  const std::vector<double> getNumberArrayField(const std::string &name) const;
  const std::vector<bool> getBooleanArrayField(const std::string &name) const;
};
} // namespace firefly::db