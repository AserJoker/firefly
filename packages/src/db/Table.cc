#include "db/Table.hpp"
#include "db/Record.hpp"
using namespace firefly;
using namespace firefly::db;

Table::Table(const std::string &name, const std::string &ns,
             const std::vector<core::AutoPtr<Field>> &fields)
    : Entity(name, ns), _fields(fields) {}
const std::vector<core::AutoPtr<Field>> &Table::getFields() const {
  return _fields;
}
const std::vector<core::AutoPtr<Record>> &Table::getRecords() const {
  return _records;
}
void Table::addRecord(const std::unordered_map<std::string, std::any> &record) {
  _records.push_back(new Record(_fields, record));
}