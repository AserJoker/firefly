#include "db/Record.hpp"
#include "db/Field.hpp"
#include <cstddef>
#include <fmt/format.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::db;
const std::string Record::getKey() const {
  std::string key;
  for (auto &name : _primaryKeys) {
    auto &field = _fields.at(name);
    if (!_record.contains(name)) {
      throw std::runtime_error(
          fmt::format("Primary key '{}' is not exist", name));
    }
    std::string part;
    switch (field.getType()) {
    case Field::O2M:
    case Field::M2O:
    case Field::O2O:
    case Field::M2M:
      continue;
    case Field::STRING:
      part = std::any_cast<std::string>(getField(name));
      break;
    case Field::BOOLEAN:
      part = std::to_string(std::any_cast<bool>(getField(name)));
      break;
    case Field::FLOAT:
      part = std::to_string(std::any_cast<double_t>(getField(name)));
      break;
    case Field::INTEGER:
      part = std::to_string(std::any_cast<int32_t>(getField(name)));
      break;
    case Field::ENUM:
      part = std::any_cast<std::string>(getField(name));
      break;
    }
    if (key.empty()) {
      key = part;
    } else {
      key += "#" + part;
    }
  }
  return key;
}
const bool
Record::match(const std::unordered_map<std::string, std::any> &filter) const {
  for (auto &[key, value] : filter) {
    if (!_record.contains(key)) {
      return false;
    }
    auto &raw = getField(key);
    if (raw.type() != value.type()) {
      return false;
    }
    if (raw.type() == typeid(std::string)) {
      if (std::any_cast<std::string>(raw) !=
          std::any_cast<std::string>(value)) {
        return false;
      }
    }
    if (raw.type() == typeid(int32_t)) {
      if (std::any_cast<int32_t>(raw) != std::any_cast<int32_t>(value)) {
        return false;
      }
    }
    if (raw.type() == typeid(float)) {
      if (std::any_cast<float>(raw) != std::any_cast<float>(value)) {
        return false;
      }
    }
    if (raw.type() == typeid(bool)) {
      if (std::any_cast<bool>(raw) != std::any_cast<bool>(value)) {
        return false;
      }
    }
  }
  return true;
}
Record::Record(const std::vector<Field> &fields,
               const std::unordered_map<std::string, std::any> &data,
               const std::vector<std::string> &primaryKeys)
    : _primaryKeys(primaryKeys) {
  for (auto &field : fields) {
    _fields[field.getName()] = field;
  }
  for (auto &[key, value] : data) {
    setField(key, value);
  }
}
const std::unordered_map<std::string, std::any> &Record::getData() const {
  return _record;
}
const std::any &Record::getField(const std::string &name) const {
  if (!_record.contains(name)) {
    throw std::runtime_error(fmt::format("Undefined field '{}'", name));
  }
  return _record.at(name);
}
const bool Record::hasField(const std::string &name) const {
  return _record.contains(name);
}

void Record::setField(const std::string &name, const std::any &raw) {
  auto &field = _fields.at(name);
  if (raw.type() == typeid(nullptr_t)) {
    _record.erase(name);
    return;
  }
  auto value = raw;
  if (raw.type() == typeid(const char *)) {
    value = std::string(std::any_cast<const char *>(raw));
  }
  bool validate = true;
  switch (field.getType()) {
  case Field::O2M:
  case Field::M2O:
  case Field::O2O:
  case Field::M2M:
    throw std::runtime_error(
        fmt::format("Cannot set complex field '{}'", field.getTypeName()));
  case Field::STRING:
    if (field.getAttribute().array) {
      if (value.type() != typeid(std::vector<std::string>)) {
        validate = false;
      }
    } else {
      if (value.type() != typeid(std::string)) {
        validate = false;
      }
    }
    break;
  case Field::BOOLEAN:
    if (field.getAttribute().array) {
      if (value.type() != typeid(std::vector<bool>)) {
        validate = false;
      }
    } else {
      if (value.type() != typeid(bool)) {
        validate = false;
      }
    }
    break;
  case Field::FLOAT:
    if (field.getAttribute().array) {
      if (value.type() != typeid(std::vector<float_t>)) {
        validate = false;
      }
    } else {
      if (value.type() != typeid(float_t)) {
        validate = false;
      }
    }
    break;
  case Field::INTEGER:
    if (field.getAttribute().array) {
      if (value.type() != typeid(std::vector<int32_t>)) {
        validate = false;
      }
    } else {
      if (value.type() != typeid(int32_t)) {
        validate = false;
      }
    }
    break;
  case Field::ENUM:
    if (field.getAttribute().array) {
      if (value.type() != typeid(std::vector<std::string>)) {
        validate = false;
      }
    } else {
      if (value.type() != typeid(std::string)) {
        validate = false;
      }
    }
    break;
  }
  if (!validate) {
    throw std::runtime_error(
        fmt::format("Failed to set {} value with '{}' type",
                    field.getTypeName(), value.type().name()));
  }
  _record[name] = value;
}

template <Field::TYPE type, bool array> void validateField(const Field &field) {
  if (field.getType() != type || field.getAttribute().array != array) {
    throw std::runtime_error(fmt::format(
        "Failed to get string field '{}' with '{}'", field.getId(),
        field.getTypeName() + (field.getAttribute().array ? "(ARRAY)" : "")));
  }
}

const std::string Record::getStringField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::STRING, false>(field);
  return std::any_cast<std::string>(getField(name));
}
const std::vector<std::string>
Record::getStringArrayField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::STRING, true>(field);
  return std::any_cast<std::vector<std::string>>(getField(name));
}
const int32_t Record::getIntegerField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::INTEGER, false>(field);
  return std::any_cast<int32_t>(getField(name));
}
const std::vector<int32_t>
Record::getIntegerArrayField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::INTEGER, true>(field);
  return std::any_cast<std::vector<int32_t>>(getField(name));
}
const float Record::getFloatField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::FLOAT, false>(field);
  return std::any_cast<float>(getField(name));
}
const std::vector<float>
Record::getFloatArrayField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::FLOAT, true>(field);
  return std::any_cast<std::vector<float>>(getField(name));
}
const std::string Record::getEnumField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::ENUM, false>(field);
  return std::any_cast<std::string>(getField(name));
}
const std::vector<std::string>
Record::getEnumArrayField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::STRING, true>(field);
  return std::any_cast<std::vector<std::string>>(getField(name));
}
const bool Record::getBooleanField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::BOOLEAN, false>(field);
  return std::any_cast<bool>(getField(name));
}
const std::vector<bool>
Record::getBooleanArrayField(const std::string &name) const {
  auto &field = _fields.at(name);
  validateField<Field::BOOLEAN, true>(field);
  return std::any_cast<std::vector<bool>>(getField(name));
}