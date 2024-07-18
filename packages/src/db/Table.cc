#include "db/Table.hpp"
#include "db/Record.hpp"
#include <stdexcept>
#include <vector>
using namespace firefly;
using namespace firefly::db;

Table::Table(const std::string &name, const std::string &ns,
             const std::vector<core::AutoPtr<Field>> &fields,
             const std::vector<std::string> &primaryKeys)
    : Entity(name, ns), _primaryKeys(primaryKeys) {
  for (auto &field : fields) {
    _fields[field->getName()] = field;
  }
  for (auto &key : _primaryKeys) {
    if (!_fields.contains(key)) {
      throw std::runtime_error(fmt::format("Invalid primary key '{}'", key));
    }
  }
}
const std::vector<std::string> &Table::getPrimaryKeys() const {
  return _primaryKeys;
}
const std::unordered_map<std::string, core::AutoPtr<Field>> &
Table::getFields() const {
  return _fields;
}
const std::unordered_map<std::string, core::AutoPtr<Record>> &
Table::getRecords() const {
  return _records;
}
const std::unordered_map<std::string, core::AutoPtr<Record>> Table::getRecords(
    const std::unordered_map<std::string, std::any> &filter) const {
  std::unordered_map<std::string, core::AutoPtr<Record>> result;
  for (auto &[key, record] : _records) {
    if (record->match(filter)) {
      result[key] = record;
    }
  }
  return result;
}
void Table::addRecord(const std::unordered_map<std::string, std::any> &record,
                      const bool &readonly) {
  std::string key;
  auto rec = new Record(_fields, record, readonly);
  for (auto &name : _primaryKeys) {
    auto &field = _fields.at(name);
    switch (field->getType()) {
    case Field::TYPE::O2M:
    case Field::TYPE::M2O:
    case Field::TYPE::O2O:
    case Field::TYPE::M2M:
      throw std::runtime_error(fmt::format(
          "Cannot use complex field '{}' to primary", field->getName()));
    default:
      if (key.empty()) {
        key = rec->toString(field->getName());
      } else {
        key += "," + rec->toString(field->getName());
      }
    }
  }
  _records[key] = rec;
}
