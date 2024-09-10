#include "database/Table.hpp"
#include "exception/ValidateException.hpp"
#include <fmt/core.h>
using namespace firefly;
using namespace firefly::database;

std::unordered_map<std::string, std::function<core::AutoPtr<Table>(
                                    const core::AutoPtr<Metadata> &)>>
    Table::_drivers;

Table::Table(const core::AutoPtr<Metadata> &metadata) : _metadata(metadata) {}
const uint32_t Table::getLength() const { return (uint32_t)_records.size(); }
const core::AutoPtr<Metadata> &Table::getMetadata() const { return _metadata; }
core::AutoPtr<Record> Table::getRecord(const uint32_t &index) {
  if (index >= _records.size()) {
    return nullptr;
  }
  return _records[index];
}
const core::AutoPtr<Record> Table::getRecord(const uint32_t &index) const {
  if (index >= _records.size()) {
    return nullptr;
  }
  return _records[index];
}
core::AutoPtr<Record> Table::queryOne(core::AutoPtr<Record> query) {
  for (auto &record : _records) {
    if (record->match(query)) {
      return record;
    }
  }
  return nullptr;
}
std::vector<core::AutoPtr<Record>>
Table::queryList(core::AutoPtr<Record> query) {
  std::vector<core::AutoPtr<Record>> result;
  for (auto &record : _records) {
    if (record->match(query)) {
      result.push_back(record);
    }
  }
  return result;
}
core::AutoPtr<Record> Table::insertOne(core::AutoPtr<Record> query) {
  for (auto &field : getMetadata()->getFields()) {
    if (field.isRequired()) {
      if (!query->hasField(field.getName()) ||
          query->getField(field.getName())->isNil()) {
        throw exception::ValidateException(fmt::format(
            "Faield to insert record to '{}.{}',field '{}' is required",
            _metadata->getNamespace(), _metadata->getName(), field.getName()));
      }
    }
  }
  auto key = query->getKey();
  if (_indices.contains(key)) {
    throw exception::ValidateException(
        fmt::format("Duplicate primary key: '{}'", key));
  }
  _records.push_back(query);
  _indices[key] = (uint32_t)_records.size() - 1;
  return query;
}
core::AutoPtr<Record> Table::updateOne(core::AutoPtr<Record> query) {
  for (auto &field : getMetadata()->getFields()) {
    if (field.isRequired()) {
      if (query->getField(field.getName())->isNil()) {
        throw exception::ValidateException(fmt::format(
            "Faield to update record to '{}.{}',field '{}' is required",
            _metadata->getNamespace(), _metadata->getName(), field.getName()));
      }
    }
    if (field.isReadonly()) {
      if (query->hasField(field.getName())) {
        throw exception::ValidateException(fmt::format(
            "Faield to update record to '{}.{}',field '{}' is readonly,now "
            "give value '{}'",
            _metadata->getNamespace(), _metadata->getName(), field.getName(),
            query->getField(field.getName())->toString()));
      }
    }
  }
  auto key = query->getKey();
  if (_indices.contains(key)) {
    auto &record = _records[_indices[key]];
    record->merge(query);
    return record;
  }
  throw exception::ValidateException("Failed to match record");
}
core::AutoPtr<Record> Table::deleteOne(core::AutoPtr<Record> query) {
  auto key = query->getKey();
  if (_indices.contains(key)) {
    auto index = _indices[key];
    auto record = _records[index];
    _records.erase(_records.begin() + index);
    for (auto i = index; i < _records.size(); i++) {
      _indices[_records[i]->getKey()] = i;
    }
    _indices.erase(key);
    return record;
  }
  return nullptr;
}

core::AutoPtr<Record> Table::insertOrUpdateOne(core::AutoPtr<Record> query) {
  auto key = query->getKey();
  if (_indices.contains(key)) {
    return updateOne(query);
  }
  return insertOne(query);
}
core::AutoPtr<Table> Table::create(const std::string &name,
                                   const core::AutoPtr<Metadata> &metadata) {
  if (!_drivers.contains(name)) {
    throw exception::ValidateException(
        fmt::format("Unknown driver named '{}'", name));
  }
  return _drivers.at(name)(metadata);
}