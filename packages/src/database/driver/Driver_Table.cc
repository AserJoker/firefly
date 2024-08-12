#include "database/driver/Driver_Table.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "database/Database.hpp"
#include "database/Field.hpp"
#include "database/Metadata.hpp"
#include <stdexcept>
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::database;
core::AutoPtr<Metadata>
Driver_Table::createMetadata(core::AutoPtr<Record> query) {
  std::string name = query->getField("name")->getStringValue();
  std::string ns = query->getField("namespace")->getStringValue();
  std::vector<Field> fields;
  auto db = core::Singleton<Database>::instance();
  auto driver_field = db->getTable("firefly.field");
  std::unordered_map<std::string, core::AutoPtr<Value>> field_query;
  field_query["namespace"] = new Value(fmt::format("{}.{}", ns, name));
  auto records = driver_field->queryList(
      new Record(driver_field->getMetadata(), field_query));
  for (auto &record : records) {
    fields.push_back(createField(record));
  }
  std::string driver;
  if (query->hasField("driver")) {
    driver = query->getField("driver")->getStringValue();
  }
  std::vector<std::string> primaryKeys;
  if (query->hasField("primaryKeys")) {
    primaryKeys = query->getField("primaryKeys")->getStringArrayValue();
  }
  return new Metadata(name, ns, fields, primaryKeys, driver);
}
core::AutoPtr<Record> Driver_Table::insertOne(core::AutoPtr<Record> query) {
  auto result = Table::insertOne(query);
  if (query->getKey() != "firefly.table" &&
      query->getKey() != "firefly.field") {
    auto metadata = createMetadata(query);
    auto db = core::Singleton<Database>::instance();
    db->createTable(metadata);
  }
  return result;
}
core::AutoPtr<Record> Driver_Table::updateOne(core::AutoPtr<Record> query) {
  auto result = Table::insertOne(query);
  if (query->getKey() != "firefly.table" &&
      query->getKey() != "firefly.field") {
    auto metadata = createMetadata(query);
    auto db = core::Singleton<Database>::instance();
    db->updateTable(metadata);
  } else {
    throw std::runtime_error("Cannot update inner table");
  }
  return result;
}
core::AutoPtr<Record> Driver_Table::deleteOne(core::AutoPtr<Record> query) {
  auto result = Table::insertOne(query);
  if (query->getKey() != "firefly.table" &&
      query->getKey() != "firefly.field") {
    auto metadata = createMetadata(query);
    auto db = core::Singleton<Database>::instance();
    auto name =
        fmt::format("{}.{}", metadata->getName(), metadata->getNamespace());
    db->deleteTable(name);
  } else {
    throw std::runtime_error("Cannot delete inner table");
  }
  return result;
}
Field::TYPE createFieldType(const std::string &source) {
  if (source == "STRING") {
    return Field::TYPE::STRING;
  }
  if (source == "BOOLEAN") {
    return Field::TYPE::BOOLEAN;
  }
  if (source == "INTEGER") {
    return Field::TYPE::INTEGER;
  }
  if (source == "NUMBER") {
    return Field::TYPE::NUMBER;
  }
  throw std::runtime_error(fmt::format("Unknown field type '{}'", source));
}
Field Driver_Table::createField(core::AutoPtr<Record> record) {
  Field::TYPE type =
      createFieldType(record->getField("type")->getStringValue());
  std::string name = record->getField("name")->getStringValue();
  std::string ns = record->getField("namespace")->getStringValue();
  bool required = false;
  if (record->hasField("required")) {
    required = record->getField("required")->getBooleanValue();
  }
  bool readonly = false;
  if (record->hasField("readonly")) {
    readonly = record->getField("readonly")->getBooleanValue();
  }
  bool array = false;
  if (record->hasField("array")) {
    array = record->getField("array")->getBooleanValue();
  }
  return Field(type, name, ns, required, readonly, array);
}

Driver_Table::Driver_Table(const core::AutoPtr<Metadata> &metadata)
    : Table(metadata) {}