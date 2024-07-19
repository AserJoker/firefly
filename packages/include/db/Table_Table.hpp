#pragma once
#include "core/AutoPtr.hpp"
#include "db/Table.hpp"
#include <unordered_map>
namespace firefly::db {
class Table_Table : public Table {
private:
  static inline std::unordered_map<std::string, core::AutoPtr<Table>> _tables;

public:
  static core::AutoPtr<Table> getTable(const std::string &id);

public:
  const core::AutoPtr<Record>
  insertOne(const std::unordered_map<std::string, std::any> &record) override;
  const core::AutoPtr<Record>
  updateOne(const std::unordered_map<std::string, std::any> &record) override;
  const core::AutoPtr<Record>
  deleteOne(const std::unordered_map<std::string, std::any> &record) override;
};
}; // namespace firefly::db