#include "db/Table.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "db/Database.hpp"
#include "db/Entity.hpp"
#include "db/Field.hpp"
#include "db/Record.hpp"
#include <any>
#include <stdexcept>
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::db;
void Table::initialize(const std::string &name, const std::string &ns,
                       const std::vector<Field> &fields,
                       const std::vector<std::string> &primaryKeys) {
  Entity::initialize(name, ns);
  _fields = fields;
  _primaryKeys = primaryKeys;
  for (auto i = 0; i < fields.size(); i++) {
    _fieldIndices[fields[i].getName()] = i;
  }
}
void Table::initialize(const core::AutoPtr<Record> &record) {
  initialize(record->getStringField("name"),
             record->getStringField("namespace"), {},
             record->hasField("primaryKeys")
                 ? record->getStringArrayField("primaryKeys")
                 : std::vector<std::string>({"id"}));
  auto db = core::Singleton<Database>::instance();
  auto metas =
      db->getTable("firefly.field")->queryList({{"namespace", getId()}});
  for (auto i = 0; i < metas.size(); i++) {
    _fields.emplace_back(metas[i]);
    _fieldIndices[_fields[i].getName()] = i;
  }
}
const std::vector<Field> &Table::getFields() const { return _fields; }
const Field &Table::getField(const std::string &name) const {
  return _fields.at(_fieldIndices.at(name));
}
const bool Table::hasField(const std::string &name) const {
  return _fieldIndices.contains(name);
}
const std::vector<std::string> &Table::getPrimaryKeys() const {
  return _primaryKeys;
};
std::vector<core::AutoPtr<Record>> Table::queryList(
    const std::unordered_map<std::string, std::any> &filter) const {
  std::vector<core::AutoPtr<Record>> result;
  if (filter.empty()) {
    for (auto &[_, record] : _records) {
      result.push_back(record);
    }
  } else {
    for (auto &[_, record] : _records) {
      if (record->match(filter)) {
        result.push_back(record);
      }
    }
  }
  return result;
}
const core::AutoPtr<Record>
Table::queryOne(const std::unordered_map<std::string, std::any> &record) const {
  auto result = new Record(_fields, record, _primaryKeys);
  auto key = result->getKey();
  if (!_records.contains(key)) {
    return nullptr;
  }
  return _records.at(key);
}
const core::AutoPtr<Record>
Table::insertOne(const std::unordered_map<std::string, std::any> &record) {
  auto result = new Record(_fields, record, _primaryKeys);
  auto key = result->getKey();
  if (_records.contains(key)) {
    throw std::runtime_error(fmt::format(
        "Failed to insert record to '{}',record '{}' is exist", getId(), key));
  }
  _records[key] = result;
  return result;
}
const core::AutoPtr<Record>
Table::updateOne(const std::unordered_map<std::string, std::any> &record) {
  auto result = new Record(_fields, record, _primaryKeys);
  auto key = result->getKey();
  if (!_records.contains(key)) {
    throw std::runtime_error(
        fmt::format("Failed to insert record to '{}',record '{}' is not exist",
                    getId(), key));
  }
  auto &current = _records.at(key);
  for (auto &field : _fields) {
    if (record.contains(field.getName())) {
      current->setField(field.getName(), result->getField(field.getName()));
    }
  }
  return current;
}
const core::AutoPtr<Record> Table::insertOrUpdateOne(
    const std::unordered_map<std::string, std::any> &record) {
  if (queryOne(record) != nullptr) {
    return updateOne(record);
  }
  return insertOne(record);
}
const core::AutoPtr<Record>
Table::deleteOne(const std::unordered_map<std::string, std::any> &record) {
  auto result = new Record(_fields, record, _primaryKeys);
  auto key = result->getKey();
  if (!_records.contains(key)) {
    throw std::runtime_error(
        fmt::format("Failed to insert record to '{}',record '{}' is not exist",
                    getId(), key));
  }
  auto current = _records.at(key);
  _records.erase(key);
  return current;
}