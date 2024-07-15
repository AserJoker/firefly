#pragma once
#include "Field.hpp"
#include "Record.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::db {
class Table : public core::Object {
private:
  std::vector<core::AutoPtr<Field>> _fields;
  std::vector<std::string> _primaryKeys;
  std::string _name;
  std::vector<core::AutoPtr<Record>> _records;

public:
  Table(const std::string &name,
        const std::vector<core::AutoPtr<Field>> &fields,
        const std::vector<std::string> &primaryKeys,
        const std::vector<std::unordered_map<std::string, std::any>> &records =
            {});
  const std::string &getName() const;
  const std::vector<core::AutoPtr<Record>> &getRecords() const;
  std::vector<core::AutoPtr<Record>>
  filter(std::function<bool(const std::vector<core::AutoPtr<Field>> &fields,
                            const core::AutoPtr<Record> &record)> &func);
  const uint32_t getLngth() const;
  const std::vector<core::AutoPtr<Field>> &getFields() const;
  void addRecord(const std::vector<std::unordered_map<std::string, std::any>>
                     &records = {});
  void removeRecord(const std::vector<core::AutoPtr<Record>> &records);
};
} // namespace firefly::db