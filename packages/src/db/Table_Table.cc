#include "db/Table_Table.hpp"
#include "core/AutoPtr.hpp"
#include "db/Table.hpp"
using namespace firefly;
using namespace firefly::db;
core::AutoPtr<Table> Table_Table::getTable(const std::string &id) {
  if (!_tables.contains(id)) {
    return nullptr;
  }
  return _tables.at(id);
}

const core::AutoPtr<Record> Table_Table::insertOne(
    const std::unordered_map<std::string, std::any> &record) {
  auto result = Table::insertOne(record);
  auto id = result->getStringField("id");
  auto fields = getTable("firefly.field")->queryList({{"namespace", id}});
  for (auto &field : fields) {
    
  }
  return result;
}
const core::AutoPtr<Record> Table_Table::updateOne(
    const std::unordered_map<std::string, std::any> &record) {
  auto result = Table::updateOne(record);
  auto id = result->getStringField("id");
  return result;
}
const core::AutoPtr<Record> Table_Table::deleteOne(
    const std::unordered_map<std::string, std::any> &record) {
  auto result = Table::deleteOne(record);
  auto id = result->getStringField("id");
  Table_Table::_tables.erase(id);
  return result;
}