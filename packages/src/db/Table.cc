#include "db/Table.hpp"
#include "db/Record.hpp"
using namespace firefly;
using namespace firefly::db;

Table::Table(const std::string &name, const std::string &ns,
             const std::vector<core::AutoPtr<Field>> &fields)
    : Entity(name, ns) {
  for (auto &field : fields) {
    _fields[field->getName()] = field;
  }
}
const std::unordered_map<std::string, core::AutoPtr<Field>> &
Table::getFields() const {
  return _fields;
}
const std::vector<core::AutoPtr<Record>> &Table::getRecords() const {
  return _records;
}
const std::vector<core::AutoPtr<Record>> Table::getRecords(
    const std::unordered_map<std::string, std::any> &filter) const {
  std::vector<core::AutoPtr<Record>> result;
  for (auto &record : _records) {
    if (record->match(filter)) {
      result.push_back(record);
    }
  }
  return result;
}
void Table::addRecord(const std::unordered_map<std::string, std::any> &record) {
  _records.push_back(new Record(_fields, record));
}
