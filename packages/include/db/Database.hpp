#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "db/Table.hpp"
#include "runtime/Resource.hpp"
#include <vector>
namespace firefly::db {
class Database : public core::Object {
private:
  std::vector<core::AutoPtr<Table>> _tables;
  core::AutoPtr<Table> _tableTable;
  core::AutoPtr<Table> _tableField;

private:
  void initTableTable();
  void initFieldTable();

public:
  Database();
  void loadTable(core::AutoPtr<runtime::Resource> resource);
  void reload();
  void print();
};
} // namespace firefly::db