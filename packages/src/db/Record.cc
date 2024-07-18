#include "db/Record.hpp"
#include "db/Field.hpp"
#include <any>
#include <fmt/core.h>
#include <stdexcept>
#include <string>
using namespace firefly;
using namespace firefly::db;
Record::Record(
    const std::unordered_map<std::string, core::AutoPtr<Field>> &fields,
    const std::unordered_map<std::string, std::any> &record,
    const bool &readonly)
    : _fields(fields), _readonly(readonly) {
  for (auto &[_, field] : fields) {
    auto name = field->getName();
    if (record.contains(name)) {
      switch (field->getType()) {
      case Field::TYPE::O2M:
      case Field::TYPE::M2O:
      case Field::TYPE::O2O:
      case Field::TYPE::M2M:
        break;
      case Field::TYPE::STRING:
        if (field->isArray()
                ? record.at(name).type() != typeid(std::vector<std::string>)
                : record.at(name).type() != typeid(std::string)) {
          throw std::runtime_error(
              fmt::format("Failed to initialize field '{}' with type '{}'",
                          field->getName(), field->getTypeName()));
        } else {
          _record[name] = record.at(name);
          break;
        }
      case Field::TYPE::INTEGER:
        if (field->isArray()
                ? record.at(name).type() != typeid(std::vector<int32_t>)
                : record.at(name).type() != typeid(int32_t)) {
          throw std::runtime_error(
              fmt::format("Failed to initialize field '{}' with type '{}'",
                          field->getName(), field->getTypeName()));
        } else {
          _record[name] = record.at(name);
          break;
        }
      case Field::TYPE::NUMBER:
        if (field->isArray()
                ? record.at(name).type() != typeid(std::vector<double>)
                : record.at(name).type() != typeid(double)) {
          throw std::runtime_error(
              fmt::format("Failed to initialize field '{}' with type '{}'",
                          field->getName(), field->getTypeName()));
        } else {
          _record[name] = record.at(name);
          break;
        }
      case Field::TYPE::BOOLEAN:
        if (field->isArray()
                ? record.at(name).type() != typeid(std::vector<bool>)
                : record.at(name).type() != typeid(bool)) {
          throw std::runtime_error(
              fmt::format("Failed to initialize field '{}' with type '{}'",
                          field->getName(), field->getTypeName()));
        } else {
          _record[name] = record.at(name);
          break;
        }
      case Field::TYPE::ENUM:
        if (field->isArray()
                ? record.at(name).type() != typeid(std::vector<std::string>)
                : record.at(name).type() != typeid(std::string)) {
          throw std::runtime_error(
              fmt::format("Failed to initialize field '{}' with type '{}'",
                          field->getName(), field->getTypeName()));
        } else {
          _record[name] = record.at(name);
          break;
        }
      }
    } else if (field->isRequired()) {
      throw std::runtime_error(
          fmt::format("Field '{}' is required", field->getName()));
    }
  }
}
const bool &Record::isReadonly() const { return _readonly; }
const bool
Record::match(const std::unordered_map<std::string, std::any> &filter) const {
  for (auto &[key, value] : filter) {
    if (!_record.contains(key)) {
      return false;
    }
    auto &source = _record.at(key);
    if (source.type() != value.type()) {
      return false;
    }
    if (source.type() == typeid(std::string)) {
      return std::any_cast<std::string>(source) ==
             std::any_cast<std::string>(value);
    }
    if (source.type() == typeid(int32_t)) {
      return std::any_cast<int32_t>(source) == std::any_cast<int32_t>(value);
    }
    if (source.type() == typeid(double)) {
      return std::any_cast<double>(source) == std::any_cast<double>(value);
    }
    if (source.type() == typeid(bool)) {
      return std::any_cast<bool>(source) == std::any_cast<bool>(value);
    }
    return false;
  }
  return true;
}
const std::unordered_map<std::string, std::any> &Record::getData() const {
  return _record;
}
const std::string Record::getStringField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::STRING) {
      throw std::runtime_error(
          fmt::format("Cannot get string field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get string field '{}',field is array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<std::string>(_record.at(name));
    } else {
      return "";
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}

const std::string Record::getEnumField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::ENUM) {
      throw std::runtime_error(
          fmt::format("Cannot get enum field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get enum field '{}',field is array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<std::string>(_record.at(name));
    } else {
      return "";
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const int32_t Record::getIntegerField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::INTEGER) {
      throw std::runtime_error(
          fmt::format("Cannot get integer field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get integer field '{}',field is array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<int32_t>(_record.at(name));
    } else {
      return 0;
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const double Record::getNumberField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::NUMBER) {
      throw std::runtime_error(
          fmt::format("Cannot get number field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get number field '{}',field is array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<double>(_record.at(name));
    } else {
      return .0f;
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const bool Record::getBooleanField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::BOOLEAN) {
      throw std::runtime_error(
          fmt::format("Cannot get boolean field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get boolean field '{}',field is array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<bool>(_record.at(name));
    } else {
      return false;
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const bool Record::isNil(const std::string &name) const {
  return _record.contains(name) ? false : true;
}
void Record::cleanField(const std::string &name) {
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record.erase(name);
}

const std::vector<std::string>
Record::getStringArrayField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::STRING) {
      throw std::runtime_error(
          fmt::format("Cannot get string field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (!field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get string field '{}',field is not array", name));
    }
    if (_record.contains(name)) {
      auto &val = _record.at(name);
      return std::any_cast<std::vector<std::string>>(val);
    } else {
      return {};
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const std::vector<std::string>
Record::getEnumArrayField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::ENUM) {
      throw std::runtime_error(
          fmt::format("Cannot get enum field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (!field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get enum field '{}',field is not array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<std::vector<std::string>>(_record.at(name));
    } else {
      return {};
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const std::vector<int32_t>
Record::getIntegerArrayField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::INTEGER) {
      throw std::runtime_error(
          fmt::format("Cannot get integer field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (!field->isArray()) {
      throw std::runtime_error(fmt::format(
          "Cannot get integer field '{}',field is not array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<std::vector<int32_t>>(_record.at(name));
    } else {
      return {};
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const std::vector<double>
Record::getNumberArrayField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::NUMBER) {
      throw std::runtime_error(
          fmt::format("Cannot get double field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (!field->isArray()) {
      throw std::runtime_error(
          fmt::format("Cannot get double field '{}',field is not array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<std::vector<double>>(_record.at(name));
    } else {
      return {};
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
const std::vector<bool>
Record::getBooleanArrayField(const std::string &name) const {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field->getType() != Field::TYPE::BOOLEAN) {
      throw std::runtime_error(
          fmt::format("Cannot get boolean field '{}',field type is '{}'", name,
                      field->getTypeName()));
    }
    if (!field->isArray()) {
      throw std::runtime_error(fmt::format(
          "Cannot get boolean field '{}',field is not array", name));
    }
    if (_record.contains(name)) {
      return std::any_cast<std::vector<bool>>(_record.at(name));
    } else {
      return {};
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
void Record::setField(const std::string &name, const std::string &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->getType() != Field::TYPE::STRING) {
    throw std::runtime_error(
        fmt::format("Cannot set string field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (field->isArray()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is array", field->getId()));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record[name] = source;
}
void Record::setField(const std::string &name, const bool &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->getType() != Field::TYPE::BOOLEAN) {
    throw std::runtime_error(
        fmt::format("Cannot set string field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (field->isArray()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is array", field->getId()));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record[name] = source;
}
void Record::setField(const std::string &name, const int32_t &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->getType() != Field::TYPE::INTEGER) {
    throw std::runtime_error(
        fmt::format("Cannot set string field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (field->isArray()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is array", field->getId()));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record[name] = source;
}
void Record::setField(const std::string &name, const double &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->getType() != Field::TYPE::NUMBER) {
    throw std::runtime_error(
        fmt::format("Cannot set string field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (field->isArray()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is array", field->getId()));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record[name] = source;
}
void Record::setField(const std::string &name,
                      const std::vector<std::string> &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->getType() != Field::TYPE::STRING) {
    throw std::runtime_error(
        fmt::format("Cannot set string field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (!field->isArray()) {
    throw std::runtime_error(fmt::format(
        "Invalid operator,field '{}' is not array", field->getId()));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record[name] = source;
}
void Record::setField(const std::string &name,
                      const std::vector<bool> &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->getType() != Field::TYPE::BOOLEAN) {
    throw std::runtime_error(
        fmt::format("Cannot set boolean field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (!field->isArray()) {
    throw std::runtime_error(fmt::format(
        "Invalid operator,field '{}' is not array", field->getId()));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record[name] = source;
}
void Record::setField(const std::string &name,
                      const std::vector<int32_t> &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->getType() != Field::TYPE::INTEGER) {
    throw std::runtime_error(
        fmt::format("Cannot set integer field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (!field->isArray()) {
    throw std::runtime_error(fmt::format(
        "Invalid operator,field '{}' is not array", field->getId()));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  _record[name] = source;
}
void Record::setField(const std::string &name,
                      const std::vector<double> &source) {
  if (_readonly) {
    throw std::runtime_error("record is readonly");
  }
  if (!_fields.contains(name)) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  auto field = _fields.at(name);
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Invalid operator,field '{}' is readonly", field->getId()));
  }
  if (field->getType() != Field::TYPE::NUMBER) {
    throw std::runtime_error(
        fmt::format("Cannot set number field '{}',field type is '{}'",
                    field->getId(), field->getTypeName()));
  }
  if (!field->isArray()) {
    throw std::runtime_error(fmt::format(
        "Invalid operator,field '{}' is not array", field->getId()));
  }
  _record[name] = source;
}
const std::string Record::toString(const std::string &name) const {
  auto field = _fields.at(name);
  switch (field->getType()) {
  case Field::TYPE::O2M:
  case Field::TYPE::M2O:
  case Field::TYPE::O2O:
  case Field::TYPE::M2M:
    throw std::runtime_error(fmt::format(
        "Cannot convert complex field '{}' to string", field->getName()));
  case Field::TYPE::STRING:
    if (field->isArray()) {
      auto value = getStringArrayField(name);
      std::string result;
      for (auto &part : value) {
        std::string encoded;
        encoded += "\"";
        for (auto &ch : part) {
          if (ch == '\"') {
            encoded += "\\";
          }
          encoded += ch;
        }
        encoded += "\"";
        if (result.empty()) {
          result = encoded;
        } else {
          result += "," + encoded;
        }
      }
      return result;
    } else {
      return getStringField(name);
    }
  case Field::TYPE::INTEGER:
    if (field->isArray()) {

      auto value = getIntegerArrayField(name);
      std::string result;
      for (auto &part : value) {
        if (result.empty()) {
          result = std::to_string(part);
        } else {
          result += "," + std::to_string(part);
        }
      }
      return result;
    } else {
      return std::to_string(getIntegerField(name));
    }
  case Field::TYPE::NUMBER:
    if (field->isArray()) {
      auto value = getNumberArrayField(name);
      std::string result;
      for (auto &part : value) {
        if (result.empty()) {
          result = std::to_string(part);
        } else {
          result += ",";
          result += std::to_string(part);
        }
      }
      return result;
    } else {
      return std::to_string(getNumberField(name));
    }
  case Field::TYPE::BOOLEAN:
    if (field->isArray()) {
      auto value = getBooleanArrayField(name);
      std::string result;
      for (auto part : value) {
        if (result.empty()) {
          result = std::to_string(part);
        } else {
          result += "," + std::to_string(part);
        }
      }
      return result;
    } else {
      return std::to_string(getBooleanField(name));
    }
  case Field::TYPE::ENUM:
    if (field->isArray()) {
      auto value = getEnumArrayField(name);
      std::string result;
      for (auto &part : value) {
        if (result.empty()) {
          result = part;
        } else {
          result += "," + part;
        }
      }
      return result;
    } else {
      return getEnumField(name);
    }
  }
  throw std::runtime_error("Unkown field type");
}