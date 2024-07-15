#include "db/Table.hpp"
#include "db/Record.hpp"
using namespace firefly;
using namespace firefly::db;
Table::Table(
    const std::string &name, const std::vector<core::AutoPtr<Field>> &fields,
    const std::vector<std::string> &primaryKeys,
    const std::vector<std::unordered_map<std::string, std::any>> &records)
    : _name(name), _fields(fields), _primaryKeys(primaryKeys) {
  for (auto &record : records) {
    _records.push_back(new Record(_fields, record));
  }
}

const std::string &Table::getName() const { return _name; }
const std::vector<core::AutoPtr<Record>> &Table::getRecords() const {
  return _records;
}
std::vector<core::AutoPtr<Record>> Table::filter(
    std::function<bool(const std::vector<core::AutoPtr<Field>> &fields,
                       const core::AutoPtr<Record> &record)> &func) {
  std::vector<core::AutoPtr<Record>> records;
  for (auto &record : _records) {
    if (func(_fields, record)) {
      records.push_back(record);
    }
  }
  return records;
}
const uint32_t Table::getLngth() const { return _records.size(); }
const std::vector<core::AutoPtr<Field>> &Table::getFields() const {
  return _fields;
}
void Table::addRecord(
    const std::vector<std::unordered_map<std::string, std::any>> &records) {
  for (auto &record : records) {
    _records.push_back(new Record(_fields, record));
  }
}
void Table::removeRecord(const std::vector<core::AutoPtr<Record>> &records) {
  for (auto it = _records.begin(); it != _records.end(); it++) {
    auto &record = *it;
    if (std::find(records.begin(), records.end(), record) != records.end()) {
      _records.erase(it);
      it = _records.begin();
    }
  }
}