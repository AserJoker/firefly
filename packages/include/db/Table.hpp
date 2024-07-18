#pragma once
#include "Entity.hpp"
#include "Field.hpp"
#include "core/AutoPtr.hpp"
#include "db/Record.hpp"
#include <unordered_map>
#include <vector>
namespace firefly::db {
class Table : public Entity {

private:
  std::unordered_map<std::string, core::AutoPtr<Field>> _fields;
  std::vector<std::string> _primaryKeys;
  std::unordered_map<std::string, core::AutoPtr<Record>> _records;

public:
  Table(const std::string &name, const std::string &ns,
        const std::vector<core::AutoPtr<Field>> &fields,
        const std::vector<std::string> &primaryKeys = {"id"});
  const std::unordered_map<std::string, core::AutoPtr<Field>> &
  getFields() const;
  const std::vector<std::string> &getPrimaryKeys() const;
  const std::unordered_map<std::string, core::AutoPtr<Record>> &
  getRecords() const;
  const std::unordered_map<std::string, core::AutoPtr<Record>>
  getRecords(const std::unordered_map<std::string, std::any> &filter) const;
  void addRecord(const std::unordered_map<std::string, std::any> &record,
                 const bool &readonly = false);
};
} // namespace firefly::db