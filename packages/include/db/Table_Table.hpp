#pragma once
#include "db/Table.hpp"
namespace firefly::db {
class Table_Table : public Table {
public:
  virtual const core::AutoPtr<Record>
  insertOne(const std::unordered_map<std::string, std::any> &record);
  virtual const core::AutoPtr<Record>
  updateOne(const std::unordered_map<std::string, std::any> &record);
  virtual const core::AutoPtr<Record>
  deleteOne(const std::unordered_map<std::string, std::any> &record);
};
} // namespace firefly::db