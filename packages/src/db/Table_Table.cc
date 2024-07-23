#include "db/Table_Table.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "db/Database.hpp"
#include "db/Table.hpp"
#include <stdexcept>
using namespace firefly;
using namespace firefly::db;

const core::AutoPtr<Record> Table_Table::insertOne(
    const std::unordered_map<std::string, std::any> &record) {
  auto result = Table::insertOne(record);
  if (result->getStringField("id") != "firefly.table" &&
      result->getStringField("id") != "firefly.field") {
    auto db = core::Singleton<Database>::instance();
    db->createTable(result);
  }
  return result;
}
const core::AutoPtr<Record> Table_Table::updateOne(
    const std::unordered_map<std::string, std::any> &record) {
  auto result = Table::updateOne(record);
  auto db = core::Singleton<Database>::instance();
  db->updateTable(result);
  return result;
}
const core::AutoPtr<Record> Table_Table::deleteOne(
    const std::unordered_map<std::string, std::any> &record) {
  auto result = Table::deleteOne(record);
  auto id = result->getStringField("id");
  if (result->getStringField("id") != "firefly.table" &&
      result->getStringField("id") != "firefly.field") {
    auto db = core::Singleton<Database>::instance();
    db->deleteTable(id);
    return result;
  }
  throw std::runtime_error("Invalid operator: cannot delete metadata table");
}