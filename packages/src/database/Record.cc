#include "database/Record.hpp"
#include <stdexcept>
using namespace firefly;
using namespace firefly::database;

Record::Record(
    const core::AutoPtr<Metadata> &metadata,
    const std::unordered_map<std::string, core::AutoPtr<Value>> &data)
    : _metadata(metadata), _data(data) {}
core::AutoPtr<Value> Record::getField(const std::string &name) {
  if (_data.contains(name)) {
    return _data.at(name);
  }
  return nullptr;
}
const core::AutoPtr<Metadata> &Record::getMetadata() const { return _metadata; }
const std::string Record::getKey() const {
  std::string result;
  for (auto &key : _metadata->getPrimaryKeys()) {
    if (!result.empty()) {
      result += ",";
    }
    if (!_data.contains(key)) {
      throw std::runtime_error(fmt::format(
          "Failed to get record key, primary key '{}' is undefined", key));
    }
    result += _data.at(key)->toString();
  }
  return result;
}
const bool Record::hasField(const std::string &name) const {
  return _data.contains(name);
}

const core::AutoPtr<Value> Record::getField(const std::string &name) const {
  if (_data.contains(name)) {
    return _data.at(name);
  }
  return nullptr;
}
const bool Record::match(const core::AutoPtr<Record> record) const {
  for (auto &field : getMetadata()->getFields()) {
    if (record->hasField(field.getName())) {
      if (!hasField(field.getName())) {
        return false;
      }
      if (!_data.at(field.getName())
               ->isEqual(record->getField(field.getName()))) {
        return false;
      }
    }
  }
  return true;
}
void Record::merge(const core::AutoPtr<Record> another) {
  for (auto &field : _metadata->getFields()) {
    if (another->hasField(field.getName())) {
      if (another->getField(field.getName())->isNil()) {
        _data.erase(field.getName());
      } else if (!_data[field.getName()]->isEqual(
                     another->getField(field.getName()))) {
        _data[field.getName()] = another->getField(field.getName());
      }
    }
  }
}