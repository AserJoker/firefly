#include "database/Database.hpp"
#include "core/AutoPtr.hpp"
#include "database/Field.hpp"
#include "database/Metadata.hpp"
#include "database/driver/Driver_Table.hpp"
#include <stdexcept>
using namespace firefly;
using namespace firefly::database;

void Database::initialize() {
  Table::setDriver<Driver_Table>(Driver_Table::DRIVER_NAME);
  core::AutoPtr metaTable = new Metadata(
      "table", "metadata",
      {
          Field(Field::TYPE::STRING, "id", "firefly.table", true, true),
          Field(Field::TYPE::STRING, "name", "firefly.table", true, true),
          Field(Field::TYPE::STRING, "namespace", "firefly.table", true, true),
          Field(Field::TYPE::STRING, "driver", "firefly.table", false, true),
          Field(Field::TYPE::STRING, "primaryKeys", "firefly.table", true, true,
                true),
      },
      {"id"}, Driver_Table::DRIVER_NAME);
  core::AutoPtr metaField = new Metadata(
      "field", "metadata",
      {
          Field(Field::TYPE::STRING, "id", "firefly.field", true, true),
          Field(Field::TYPE::STRING, "name", "firefly.field", true, true),
          Field(Field::TYPE::STRING, "namespace", "firefly.field", true, true),
          Field(Field::TYPE::STRING, "type", "firefly.field", true, true),
          Field(Field::TYPE::BOOLEAN, "required", "firefly.field", false, true),
          Field(Field::TYPE::BOOLEAN, "readonly", "firefly.field", false, true),
          Field(Field::TYPE::BOOLEAN, "array", "firefly.field", false, true),
      });
  core::AutoPtr driverTable = new Driver_Table(metaTable);
  core::AutoPtr driverField = new Table(metaField);
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.table.id"))},
                     {"name", new Value(std::string("id"))},
                     {"namespace", new Value(std::string("firefly.table"))},
                     {"type", new Value(std::string("STRING"))},
                     {"required", new Value(true)},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.table.name"))},
                     {"name", new Value(std::string("name"))},
                     {"namespace", new Value(std::string("firefly.table"))},
                     {"type", new Value(std::string("STRING"))},
                     {"required", new Value(true)},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.table.namespace"))},
                     {"name", new Value(std::string("namespace"))},
                     {"namespace", new Value(std::string("firefly.table"))},
                     {"type", new Value(std::string("STRING"))},
                     {"required", new Value(true)},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.table.driver"))},
                     {"name", new Value(std::string("driver"))},
                     {"namespace", new Value(std::string("firefly.table"))},
                     {"type", new Value(std::string("STRING"))},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(new Record(
      driverField->getMetadata(),
      {
          {"id", new Value(std::string("firefly.table.primaryKeys"))},
          {"name", new Value(std::string("primaryKeys"))},
          {"namespace", new Value(std::string("firefly.table"))},
          {"type", new Value(std::string("STRING"))},
          {"readonly", new Value(true)},
          {"required", new Value(true)},
          {"array", new Value(true)},
      }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.field.id"))},
                     {"name", new Value(std::string("id"))},
                     {"namespace", new Value(std::string("firefly.field"))},
                     {"type", new Value(std::string("STRING"))},
                     {"required", new Value(true)},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.field.name"))},
                     {"name", new Value(std::string("name"))},
                     {"namespace", new Value(std::string("firefly.field"))},
                     {"type", new Value(std::string("STRING"))},
                     {"required", new Value(true)},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.field.namespace"))},
                     {"name", new Value(std::string("namespace"))},
                     {"namespace", new Value(std::string("firefly.field"))},
                     {"type", new Value(std::string("STRING"))},
                     {"required", new Value(true)},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.field.type"))},
                     {"name", new Value(std::string("type"))},
                     {"namespace", new Value(std::string("firefly.field"))},
                     {"type", new Value(std::string("STRING"))},
                     {"required", new Value(true)},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.field.required"))},
                     {"name", new Value(std::string("required"))},
                     {"namespace", new Value(std::string("firefly.field"))},
                     {"type", new Value(std::string("BOOLEAN"))},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.field.readonly"))},
                     {"name", new Value(std::string("readonly"))},
                     {"namespace", new Value(std::string("firefly.field"))},
                     {"type", new Value(std::string("BOOLEAN"))},
                     {"readonly", new Value(true)},
                 }));
  driverField->insertOne(
      new Record(driverField->getMetadata(),
                 {
                     {"id", new Value(std::string("firefly.field.array"))},
                     {"name", new Value(std::string("array"))},
                     {"namespace", new Value(std::string("firefly.field"))},
                     {"type", new Value(std::string("BOOLEAN"))},
                     {"readonly", new Value(true)},
                 }));
  driverTable->insertOne(new Record(
      driverTable->getMetadata(),
      {
          {"id", new Value(std::string("firefly.field"))},
          {"name", new Value(std::string("field"))},
          {"namespace", new Value(std::string("firefly"))},
          {"primaryKeys", new Value(std::vector<std::string>({"id"}))},
      }));
  driverTable->insertOne(new Record(
      driverTable->getMetadata(),
      {
          {"id", new Value(std::string("firefly.table"))},
          {"name", new Value(std::string("table"))},
          {"namespace", new Value(std::string("firefly"))},
          {"driver", new Value(std::string(Driver_Table::DRIVER_NAME))},
          {"primaryKeys", new Value(std::vector<std::string>({"id"}))},
      }));
  _tables["firefly.table"] = driverTable;
  _tables["firefly.field"] = driverField;
}

void Database::createTable(core::AutoPtr<Metadata> metadata) {
  auto key =
      fmt::format("{}.{}", metadata->getNamespace(), metadata->getName());
  if (metadata->getDriver().empty()) {
    _tables[key] = new Table(metadata);
  } else {
    _tables[key] = Table::create(metadata->getDriver(), metadata);
  }
}
void Database::deleteTable(const std::string &name) { _tables.erase(name); }
void Database::updateTable(core::AutoPtr<Metadata> metadata) {
  auto key =
      fmt::format("{}.{}", metadata->getNamespace(), metadata->getName());
  if (_tables.contains(key)) {
    _tables[key] = Table::create(metadata->getDriver(), metadata);
  } else {
    throw std::runtime_error(fmt::format("Unknown table named '{}'", key));
  }
}
core::AutoPtr<Table> Database::getTable(const std::string &name) {
  if (_tables.contains(name)) {
    return _tables.at(name);
  }
  return nullptr;
}
const core::AutoPtr<Table> Database::getTable(const std::string &name) const {
  if (_tables.contains(name)) {
    return _tables.at(name);
  }
  return nullptr;
}

const std::unordered_map<std::string, core::AutoPtr<Table>> &
Database::getTables() const {
  return _tables;
}