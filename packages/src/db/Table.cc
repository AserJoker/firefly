#include "db/Table.hpp"
#include "db/Entity.hpp"
#include "db/Record.hpp"
#include <any>
#include <stdexcept>
#include <unordered_map>
using namespace firefly;
using namespace firefly::db;
Table::Table(const std::string &name, const std::string &ns,
             const std::vector<Field> &fields,
             const std::vector<std::string> &primaryKeys)
    : Entity(name, ns), _primaryKeys(primaryKeys), _fields(fields) {}
const std::vector<Field> &Table::getFields() const { return _fields; }
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