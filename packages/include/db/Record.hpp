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
  std::unordered_map<std::string, std::any> _record;
  std::vector<core::AutoPtr<Field>> _fields;

private:
  const core::AutoPtr<Field> getField(const std::string &name) const;

public:
  Record(const std::vector<core::AutoPtr<Field>> &fields,
         const std::unordered_map<std::string, std::any> &record = {});
  void setField(const std::string &name, const std::string &data);
  void setField(const std::string &name, const float &data);
  void setField(const std::string &name, const bool &data);
  void clearField(const std::string &name);
  void getField(const std::string &name, std::string *data);
  void getField(const std::string &name, float *data);
  void getField(const std::string &name, bool *data);
};
}; // namespace firefly::db