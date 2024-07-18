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
  std::unordered_map<std::string, core::AutoPtr<Field>> _fields;
  std::unordered_map<std::string, std::any> _record;
  bool _readonly;

public:
  Record(const std::unordered_map<std::string, core::AutoPtr<Field>> &fields,
         const std::unordered_map<std::string, std::any> &record = {},
         const bool &readonly = false);
  const bool &isReadonly() const;
  const bool
  match(const std::unordered_map<std::string, std::any> &filter) const;
  const bool isNil(const std::string &name) const;
  void cleanField(const std::string &name);
  const std::unordered_map<std::string, std::any> &getData() const;

  const std::string getStringField(const std::string &name) const;
  const std::string getEnumField(const std::string &name) const;
  const int32_t getIntegerField(const std::string &name) const;
  const double getNumberField(const std::string &name) const;
  const bool getBooleanField(const std::string &name) const;
  const std::vector<std::string>
  getStringArrayField(const std::string &name) const;
  const std::vector<std::string>
  getEnumArrayField(const std::string &name) const;
  const std::vector<int32_t>
  getIntegerArrayField(const std::string &name) const;
  const std::vector<double> getNumberArrayField(const std::string &name) const;
  const std::vector<bool> getBooleanArrayField(const std::string &name) const;

  void setField(const std::string &name, const std::string &source);
  void setField(const std::string &name, const bool &source);
  void setField(const std::string &name, const int32_t &source);
  void setField(const std::string &name, const double &source);
  void setField(const std::string &name,
                const std::vector<std::string> &source);
  void setField(const std::string &name, const std::vector<bool> &source);
  void setField(const std::string &name, const std::vector<int32_t> &source);
  void setField(const std::string &name, const std::vector<double> &source);
  const std::string toString(const std::string &name) const;
};
} // namespace firefly::db