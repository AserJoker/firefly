#include "database/Database.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "database/Field.hpp"
#include "database/Metadata.hpp"
#include "database/Record.hpp"
#include "database/Value.hpp"
#include "database/driver/Driver_Table.hpp"
#include "exception/ValidateException.hpp"
#include "libxml/parser.h"
#include "runtime/Media.hpp"
#include <cinttypes>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <unordered_map>
using namespace firefly;
using namespace firefly::database;

void Database::initialize() {
  Table::setDriver<Driver_Table>(Driver_Table::DRIVER_NAME);
  core::AutoPtr metaTable = new Metadata(
      "table", "firefly",
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
      "field", "firefly",
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
    throw exception::ValidateException(
        fmt::format("Unknown table named '{}'", key));
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
void Database::load() {
  auto media = core::Singleton<runtime::Media>::instance();
  auto items = media->scan("table");
  std::vector<std::string> fields;
  std::vector<std::string> tables;
  std::vector<std::string> records;
  for (auto &item : items) {
    if (item.ends_with(".field.xml")) {
      fields.push_back(item);
    } else if (item.ends_with(".table.xml")) {
      tables.push_back(item);
    } else {
      records.push_back(item);
    }
  }
  for (auto &item : fields) {
    auto data = media->load(item)->read();
    loadFromXML(data);
  }
  for (auto &item : tables) {
    auto data = media->load(item)->read();
    loadFromXML(data);
  }
  for (auto &item : records) {
    auto data = media->load(item)->read();
    loadFromXML(data);
  }
}

void Database::loadFromXML(const core::AutoPtr<core::Buffer> &buffer) {
  std::string source((char *)buffer->getData(), buffer->getSize());
  auto doc = xmlParseMemory((const char *)buffer->getData(), buffer->getSize());
  auto root = xmlDocGetRootElement(doc);
  std::string tableName = (const char *)xmlGetProp(root, (xmlChar *)"id");
  auto table = getTable(tableName);
  auto meta = table->getMetadata();
  auto record = root->children;
  while (record != nullptr) {
    if (xmlStrcmp(record->name, (const xmlChar *)"record") == 0) {
      std::unordered_map<std::string, core::AutoPtr<Value>> query;
      auto field = record->children;
      while (field != nullptr) {
        if (xmlStrcmp(field->name, (const xmlChar *)"field") == 0) {
          std::string name =
              (const char *)xmlGetProp(field, (const xmlChar *)"name");
          auto metaField = meta->getField(name);
          std::string value =
              (const char *)xmlGetProp(field, (const xmlChar *)"value");
          if (value != "nil") {
            if (metaField.isArray()) {
              switch (metaField.getType()) {
              case Field::TYPE::STRING: {
                std::vector<std::string> val;
                std::string item;
                for (auto &c : value) {
                  if (c == ',') {
                    if (!item.empty()) {
                      val.push_back(item);
                    }
                    item.clear();
                  } else {
                    item += c;
                  }
                }
                if (!item.empty()) {
                  val.push_back(item);
                }
                query[name] = new Value(val);
              } break;
              case Field::TYPE::INTEGER: {
                char *end = 0;
                std::vector<int32_t> val;
                std::string item;
                for (auto &c : value) {
                  if (c == ',') {
                    if (!item.empty()) {
                      val.push_back((int)strtoimax(item.c_str(), &end, 10));
                    }
                    item.clear();
                  } else {
                    item += c;
                  }
                }
                if (!item.empty()) {
                  val.push_back((int)strtoimax(item.c_str(), &end, 10));
                }
                query[name] = new Value(val);
              } break;
              case Field::TYPE::NUMBER: {
                char *end = 0;
                std::vector<double> val;
                std::string item;
                for (auto &c : value) {
                  if (c == ',') {
                    if (!item.empty()) {
                      val.push_back(strtod(item.c_str(), &end));
                    }
                    item.clear();
                  } else {
                    item += c;
                  }
                }
                if (!item.empty()) {
                  val.push_back(strtod(item.c_str(), &end));
                }
                query[name] = new Value(val);
              } break;
              case Field::TYPE::BOOLEAN: {
                char *end = 0;
                std::vector<double> val;
                std::string item;
                for (auto &c : value) {
                  if (c == ',') {
                    if (!item.empty()) {
                      val.push_back(item == "true");
                    }
                    item.clear();
                  } else {
                    item += c;
                  }
                }
                if (!item.empty()) {
                  val.push_back(item == "true");
                }
                query[name] = new Value(val);
              } break;
              }
            } else {
              switch (metaField.getType()) {
              case Field::TYPE::STRING:
                query[name] = new Value(value);
                break;
              case Field::TYPE::INTEGER: {
                char *end = 0;
                query[name] = new Value(strtoimax(value.c_str(), &end, 10));
              } break;
              case Field::TYPE::NUMBER: {
                char *end = 0;
                query[name] = new Value(strtod(value.c_str(), &end));
              } break;
              case Field::TYPE::BOOLEAN:
                query[name] = new Value(value == "true");
                break;
              }
            }
          } else {
            query[name] = new Value();
          }
        }
        field = field->next;
      }
      core::AutoPtr r = new Record(table->getMetadata(), query);
      table->insertOrUpdateOne(r);
    }
    record = record->next;
  }
  xmlFreeDoc(doc);
}