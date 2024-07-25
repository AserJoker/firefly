#include "db/Database.hpp"
#include "core/AutoPtr.hpp"
#include "core/Provider.hpp"
#include "core/Singleton.hpp"
#include "db/Field.hpp"
#include "db/Record.hpp"
#include "db/Table.hpp"
#include "db/Table_Table.hpp"
#include <fmt/core.h>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::db;
void Database::initialize() {
  auto provider = core::Singleton<core::Provider>::instance();
  provider->provide<Table, DB_DEFAULT_DRIVER>();
  provider->provide<Table_Table, DB_TABLE_DRIVER>();
  auto tableTable = new Table_Table();
  auto tableField = new Table();
  std::vector<Field> fields = {
      {"id", "firefly.field", Field::STRING,
       Field::Attribute{.required = true, .readonly = true}},
      {"name", "firefly.field", Field::STRING,
       Field::Attribute{.required = true, .readonly = true}},
      {"namespace", "firefly.field", Field::STRING,
       Field::Attribute{.required = true, .readonly = true}},
      {"type",
       "firefly.field",
       Field::ENUM,
       {"O2M", "M2O", "O2O", "M2M", "STRING", "BOOLEAN", "FLOAT", "INTEGER",
        "ENUM"},
       Field::Attribute{.required = true, .readonly = true}},
      {"required", "firefly.field", Field::BOOLEAN,
       Field::Attribute{.readonly = true}},
      {"readonly", "firefly.field", Field::BOOLEAN,
       Field::Attribute{.readonly = true}},
      {"array", "firefly.field", Field::BOOLEAN,
       Field::Attribute{.readonly = true}},
      {"options", "firefly.field", Field::STRING,
       Field::Attribute{.readonly = true, .array = true}},
      {"refModel", "firefly.field", Field::STRING,
       Field::Attribute{.readonly = true}},
      {"relatedFields", "firefly.field", Field::STRING,
       Field::Attribute{.readonly = true, .array = true}},
      {"relationFields", "firefly.field", Field::STRING,
       Field::Attribute{.readonly = true, .array = true}},
  };
  tableField->initialize("field", "firefly", fields);
  fields = {
      {"id", "firefly.table", Field::STRING,
       Field::Attribute{.required = true, .readonly = true}},
      {"name", "firefly.table", Field::STRING,
       Field::Attribute{.required = true, .readonly = true}},
      {"namespace", "firefly.table", Field::STRING,
       Field::Attribute{.required = true, .readonly = true}},
      {"primaryKeys", "firefly.table", Field::STRING,
       Field::Attribute{.required = true, .array = true}},
      {"fields",
       "firefly.table",
       Field::O2M,
       "firefly.field",
       {"namespace"},
       {"id"},
       Field::Attribute{.required = true}},
      {"driver", "firefly.table", Field::STRING},

  };
  tableTable->initialize("table", "firefly", fields);
  _tables[tableField->getId()] = tableField;
  _tables[tableTable->getId()] = tableTable;
  tableField->insertOne({
      {"id", "firefly.field.id"},
      {"name", "id"},
      {"namespace", "firefly.field"},
      {"type", "STRING"},
      {"required", true},
  });
  tableField->insertOne({
      {"id", "firefly.field.name"},
      {"name", "name"},
      {"namespace", "firefly.field"},
      {"type", "STRING"},
      {"required", true},
  });
  tableField->insertOne({
      {"id", "firefly.field.namespace"},
      {"name", "namespace"},
      {"namespace", "firefly.field"},
      {"type", "STRING"},
      {"required", true},
  });
  tableField->insertOne({
      {"id", "firefly.field.type"},
      {"name", "type"},
      {"namespace", "firefly.field"},
      {"type", "ENUM"},
      {"options",
       std::vector<std::string>({"O2M", "M2O", "O2O", "M2M", "STRING",
                                 "BOOLEAN", "FLOAT", "INTEGER", "ENUM"})},
      {"required", true},
  });
  tableField->insertOne({
      {"id", "firefly.field.required"},
      {"name", "required"},
      {"namespace", "firefly.field"},
      {"type", "BOOLEAN"},
  });
  tableField->insertOne({
      {"id", "firefly.field.readonly"},
      {"name", "readonly"},
      {"namespace", "firefly.field"},
      {"type", "BOOLEAN"},
  });
  tableField->insertOne({
      {"id", "firefly.field.array"},
      {"name", "array"},
      {"namespace", "firefly.field"},
      {"type", "BOOLEAN"},
  });
  tableField->insertOne({
      {"id", "firefly.field.options"},
      {"name", "options"},
      {"namespace", "firefly.field"},
      {"type", "STRING"},
      {"array", true},
  });
  tableField->insertOne({
      {"id", "firefly.field.refModel"},
      {"name", "refModel"},
      {"namespace", "firefly.field"},
      {"type", "STRING"},
  });
  tableField->insertOne({
      {"id", "firefly.field.relatedFields"},
      {"name", "relatedFields"},
      {"namespace", "firefly.field"},
      {"type", "STRING"},
      {"array", true},
  });
  tableField->insertOne({
      {"id", "firefly.field.relationFields"},
      {"name", "relationFields"},
      {"namespace", "firefly.field"},
      {"type", "STRING"},
      {"array", true},
  });
  tableField->insertOne({
      {"id", "firefly.table.id"},
      {"name", "id"},
      {"namespace", "firefly.table"},
      {"type", "STRING"},
      {"required", true},
  });
  tableField->insertOne({
      {"id", "firefly.table.name"},
      {"name", "name"},
      {"namespace", "firefly.table"},
      {"type", "STRING"},
      {"required", true},
  });
  tableField->insertOne({
      {"id", "firefly.table.namespace"},
      {"name", "namespace"},
      {"namespace", "firefly.table"},
      {"type", "STRING"},
      {"required", true},
  });
  tableField->insertOne({
      {"id", "firefly.table.fields"},
      {"name", "fields"},
      {"namespace", "firefly.table"},
      {"type", "O2M"},
      {"required", true},
      {"refModel", "firefly.field"},
      {"relatedFields", std::vector<std::string>({"id"})},
      {"relationFields", std::vector<std::string>({"namespace"})},
  });
  tableField->insertOne({
      {"id", "firefly.table.driver"},
      {"name", "driver"},
      {"namespace", "firefly.table"},
      {"type", "STRING"},
  });
  tableTable->insertOne({
      {"id", "firefly.table"},
      {"name", "table"},
      {"namespace", "firefly"},
      {"primaryKeys", std::vector<std::string>({"id"})},
      {"driver", "firefly.db.Table_Table"},
  });
  tableTable->insertOne({
      {"id", "firefly.field"},
      {"name", "field"},
      {"namespace", "firefly"},
      {"primaryKeys", std::vector<std::string>({"id"})},
      {"driver", "firefly.db.Table"},
  });
}
core::AutoPtr<Table> Database::getTable(const std::string &table) {
  if (_tables.contains(table)) {
    return _tables.at(table);
  }
  return nullptr;
}

core::AutoPtr<Table>
Database::createTable(const std::string &driver, const std::string &name,
                      const std::string &ns, const std::vector<Field> &fields,
                      const std::vector<std::string> &primaryKeys) {
  auto provider = core::Singleton<core::Provider>::instance();
  auto table = provider->inject<Table>(driver, name, ns, fields, primaryKeys);
  _tables[table->getId()] = table;
  return table;
}
core::AutoPtr<Table> Database::createTable(const core::AutoPtr<Record> &meta) {
  std::string driver = DB_DEFAULT_DRIVER;
  if (meta->hasField("driver")) {
    driver = meta->getStringField("driver");
  }
  auto provider = core::Singleton<core::Provider>::instance();
  auto table = provider->inject<Table>(driver, meta);
  _tables[table->getId()] = table;
  return table;
}
core::AutoPtr<Table> Database::deleteTable(const std::string &id) {
  if (_tables.contains(id)) {
    auto table = _tables.at(id);
    _tables.erase(id);
    return table;
  }
  return nullptr;
}
core::AutoPtr<Table> Database::updateTable(const core::AutoPtr<Record> &meta) {
  auto id = meta->getStringField("id");
  if (_tables.contains(id)) {
    auto &old = _tables.at(id);
    auto driver = meta->hasField("driver") ? meta->getStringField("driver")
                                           : DB_DEFAULT_DRIVER;
    auto provider = core::Singleton<core::Provider>::instance();
    auto table = provider->inject<Table>(driver, meta);
    auto records = old->queryList();
    for (auto &record : records) {
      table->insertOne(record->getData());
    }
    _tables[id] = table;
    return table;
  }
  return nullptr;
}
std::unordered_map<std::string, std::any>
Database::resolveSchema(const Schema &schema,
                        const core::AutoPtr<Table> &driver,
                        const core::AutoPtr<Record> &record) {
  std::unordered_map<std::string, std::any> result;
  for (auto &[key, next] : schema.children) {
    auto &field = driver->getField(key);
    if (next.children.empty()) {
      if (field.getTypeName() != next.type) {
        throw std::runtime_error(
            fmt::format("Cannot convert field type '{}' to '{}'",
                        field.getTypeName(), next.type));
      } else {
        result[key] = record->getField(key);
      }
    } else {
      if (field.getType() == Field::M2O) {
        std::unordered_map<std::string, std::any> filter;
        auto &related = field.getRelatedFields();
        auto &relation = field.getRelationFields();
        for (auto i = 0; i < related.size(); i++) {
          filter[relation[i]] = record->getField(related[i]);
        }
        auto &nextDriver = _tables[field.getRefModel()];
        auto nextValue = nextDriver->queryOne(filter);
        result[key] = resolveSchema(next, nextDriver, nextValue);
      } else if (field.getType() == Field::O2M) {
        std::unordered_map<std::string, std::any> filter;
        auto &related = field.getRelatedFields();
        auto &relation = field.getRelationFields();
        for (auto i = 0; i < related.size(); i++) {
          filter[relation[i]] = record->getField(related[i]);
        }
        auto &nextDriver = _tables[field.getRefModel()];
        auto nextList = nextDriver->queryList(filter);
        std::vector<std::unordered_map<std::string, std::any>> items;
        for (auto &nextValue : nextList) {
          items.push_back(resolveSchema(next, nextDriver, nextValue));
        }
        result[key] = items;
      } else if (field.getType() == Field::M2M) {
        // TODO:
      } else if (field.getType() == Field::O2O) {
        // TODO:
      }
    }
  }
  return result;
}
std::unordered_map<std::string, std::any> Database::updateComplexField(
    const std::string &table,
    const std::unordered_map<std::string, std::any> &record) {
  auto &driver = _tables.at(table);
  auto &fields = driver->getFields();
  std::unordered_map<std::string, std::any> r;
  for (auto &[key, value] : record) {
    if (driver->hasField(key)) {
      auto &field = driver->getField(key);
      if (field.getType() <= Field::M2M) {
        if (field.getType() == Field::M2O) {
          auto next =
              std::any_cast<std::unordered_map<std::string, std::any>>(value);
          auto &related = field.getRelatedFields();
          auto &relation = field.getRelationFields();
          for (auto i = 0; i < related.size(); i++) {
            next[relation[i]] = record.at(related[i]);
          }
          insertOrUpdateOne(field.getRefModel(), next);
        } else if (field.getType() == Field::O2M) {
          auto next = std::any_cast<
              std::vector<std::unordered_map<std::string, std::any>>>(value);
          auto &related = field.getRelatedFields();
          auto &relation = field.getRelationFields();
          for (auto &item : next) {
            for (auto i = 0; i < related.size(); i++) {
              item[relation[i]] = record.at(related[i]);
            }
            insertOrUpdateOne(field.getRefModel(), item);
          }
        } else if (field.getType() == Field::O2O) {
          // TODO: relation field
        } else if (field.getType() == Field::M2M) {
          // TODO: relation field
        }
      } else {
        r[key] = value;
      }
    }
  }
  return r;
}
core::AutoPtr<Record>
Database::insertOne(const std::string &table,
                    const std::unordered_map<std::string, std::any> &record) {
  auto &driver = _tables.at(table);
  return driver->insertOne(updateComplexField(table, record));
}
core::AutoPtr<Record>
Database::updateOne(const std::string &table,
                    const std::unordered_map<std::string, std::any> &record) {
  auto &driver = _tables.at(table);
  return driver->updateOne(updateComplexField(table, record));
}
core::AutoPtr<Record>
Database::deleteOne(const std::string &table,
                    const std::unordered_map<std::string, std::any> &record) {
  auto &driver = _tables.at(table);
  return driver->deleteOne(record);
}
core::AutoPtr<Record> Database::insertOrUpdateOne(
    const std::string &table,
    const std::unordered_map<std::string, std::any> &record) {
  auto &driver = _tables.at(table);
  return driver->insertOrUpdateOne(record);
}
core::AutoPtr<Record>
Database::queryOne(const std::string &table,
                   const std::unordered_map<std::string, std::any> &filter,
                   const Database::Schema &schema) {
  auto &driver = _tables.at(table);
  auto raw = driver->queryOne(filter);
  std::unordered_map<std::string, std::any> record =
      resolveSchema(schema, driver, raw);
  return new Record(driver->getFields(), record, driver->getPrimaryKeys());
}
std::vector<core::AutoPtr<Record>>
Database::queryList(const std::string &table,
                    const std::unordered_map<std::string, std::any> &filter,
                    const Database::Schema &schema) {
  auto &driver = _tables.at(table);
  auto raw = driver->queryList(filter);
  std::vector<core::AutoPtr<Record>> result;
  for (auto &record : raw) {
    result.push_back(new Record(driver->getFields(),
                                resolveSchema(schema, driver, record),
                                driver->getPrimaryKeys()));
  }
  return result;
}
void Database::print() {
  for (auto &[_, table] : _tables) {
    auto &fields = table->getFields();
    auto records = table->queryList();
    std::cout << "#### " << table->getId() << std::endl;
    std::cout << "|";
    for (auto &field : fields) {
      std::cout << field.getName() << "|";
    }
    std::cout << std::endl;
    std::cout << "|";
    for (auto i = 0; i < fields.size(); i++) {
      std::cout << "-|";
    }
    std::cout << std::endl;
    for (auto &record : records) {
      for (auto &field : fields) {
        if (record->hasField(field.getName())) {
          switch (field.getType()) {
          case Field::O2M:
          case Field::M2O:
          case Field::O2O:
          case Field::M2M:
            break;
          case Field::STRING: {
            if (field.getAttribute().array) {
              auto data = record->getStringArrayField(field.getName());
              for (auto i = 0; i < data.size(); i++) {
                if (i != 0)
                  std::cout << ",";
                std::cout << data[i];
              }
            } else {
              std::cout << record->getStringField(field.getName());
            }
          } break;
          case Field::BOOLEAN: {
            if (field.getAttribute().array) {
              auto data = record->getBooleanArrayField(field.getName());
              for (auto i = 0; i < data.size(); i++) {
                if (i != 0)
                  std::cout << ",";
                std::cout << data[i];
              }
            } else {
              std::cout << record->getBooleanField(field.getName());
            }
          } break;
          case Field::FLOAT: {
            if (field.getAttribute().array) {
              auto data = record->getFloatArrayField(field.getName());
              for (auto i = 0; i < data.size(); i++) {
                if (i != 0)
                  std::cout << ",";
                std::cout << data[i];
              }
            } else {
              std::cout << record->getFloatField(field.getName());
            }
          } break;
          case Field::INTEGER: {
            if (field.getAttribute().array) {
              auto data = record->getIntegerArrayField(field.getName());
              for (auto i = 0; i < data.size(); i++) {
                if (i != 0)
                  std::cout << ",";
                std::cout << data[i];
              }
            } else {
              std::cout << record->getIntegerField(field.getName());
            }
          } break;
          case Field::ENUM: {
            if (field.getAttribute().array) {
              auto data = record->getEnumArrayField(field.getName());
              for (auto i = 0; i < data.size(); i++) {
                if (i != 0)
                  std::cout << ",";
                std::cout << data[i];
              }
            } else {
              std::cout << record->getEnumField(field.getName());
            }
          } break;
          }
        }
        std::cout << "|";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}