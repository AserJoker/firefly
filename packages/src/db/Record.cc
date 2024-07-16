#include "db/Record.hpp"
#include "db/Field.hpp"
#include <any>
#include <fmt/core.h>
#include <sstream>
#include <stdexcept>
#include <string>
using namespace firefly;
using namespace firefly::db;
Record::Record(
    const std::unordered_map<std::string, core::AutoPtr<Field>> &fields,
    const std::unordered_map<std::string, std::any> &record)
    : _fields(fields) {
  for (auto &[_, field] : fields) {
    auto name = field->getName();
    if (record.contains(name)) {
      if (field->isArray()) {
        if (record.at(name).type() != typeid(std::string)) {
          throw std::runtime_error(
              fmt::format("Failed to initialize field '{}' with type '{}'",
                          field->getName(), field->getTypeName()));
        } else {
          _record[name] = record.at(name);
          break;
        }
      } else {
        switch (field->getType()) {
        case Field::TYPE::O2M:
        case Field::TYPE::M2O:
        case Field::TYPE::O2O:
        case Field::TYPE::M2M:
          break;
        case Field::TYPE::STRING:
          if (record.at(name).type() != typeid(std::string)) {
            throw std::runtime_error(
                fmt::format("Failed to initialize field '{}' with type '{}'",
                            field->getName(), field->getTypeName()));
          } else {
            _record[name] = record.at(name);
            break;
          }
        case Field::TYPE::INTEGER:
          if (record.at(name).type() != typeid(int32_t)) {
            throw std::runtime_error(
                fmt::format("Failed to initialize field '{}' with type '{}'",
                            field->getName(), field->getTypeName()));
          } else {
            _record[name] = record.at(name);
            break;
          }
        case Field::TYPE::NUMBER:
          if (record.at(name).type() != typeid(double)) {
            throw std::runtime_error(
                fmt::format("Failed to initialize field '{}' with type '{}'",
                            field->getName(), field->getTypeName()));
          } else {
            _record[name] = record.at(name);
            break;
          }
        case Field::TYPE::BOOLEAN:
          if (record.at(name).type() != typeid(bool)) {
            throw std::runtime_error(
                fmt::format("Failed to initialize field '{}' with type '{}'",
                            field->getName(), field->getTypeName()));
          } else {
            _record[name] = record.at(name);
            break;
          }
        case Field::TYPE::ENUM:
          if (record.at(name).type() != typeid(std::string)) {
            throw std::runtime_error(
                fmt::format("Failed to initialize field '{}' with type '{}'",
                            field->getName(), field->getTypeName()));
          } else {
            _record[name] = record.at(name);
            break;
          }
        }
      }
    } else if (field->isRequired()) {
      throw std::runtime_error(
          fmt::format("Field '{}' is required", field->getName()));
    }
  }
}

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
      auto data = std::any_cast<std::string>(_record.at(name));
      std::vector<std::string> result;
      std::string part;
      bool flag = false;
      for (auto &ch : data) {
        if (ch == '\\') {
          continue;
        }
        if (ch == '\"') {
          flag = !flag;
          continue;
        }
        if (!flag && ch == ',') {
          result.push_back(part);
          part.clear();
        } else {
          part += ch;
        }
      }
      result.push_back(part);
      return result;
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
      auto data = std::any_cast<std::string>(_record.at(name));
      std::vector<std::string> result;
      std::string part;
      bool flag;
      for (auto &ch : data) {
        if (flag && ch == ',') {
          result.push_back(part);
          part.clear();
        } else {
          part += ch;
        }
      }
      result.push_back(part);
      return result;
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
      auto data = std::any_cast<std::string>(_record.at(name));
      std::vector<int32_t> result;
      std::string part;
      bool flag;
      for (auto &ch : data) {
        if (flag && ch == ',') {
          std::stringstream ss(part);
          int32_t val;
          ss >> val;
          result.push_back(val);
          part.clear();
        } else {
          part += ch;
        }
      }
      std::stringstream ss(part);
      int32_t val;
      ss >> val;
      result.push_back(val);
      return result;
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
      auto data = std::any_cast<std::string>(_record.at(name));
      std::vector<double> result;
      std::string part;
      bool flag;
      for (auto &ch : data) {
        if (flag && ch == ',') {
          std::stringstream ss(part);
          double val;
          ss >> val;
          result.push_back(val);
          part.clear();
        } else {
          part += ch;
        }
      }
      std::stringstream ss(part);
      double val;
      ss >> val;
      result.push_back(val);
      return result;
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
      auto data = std::any_cast<std::string>(_record.at(name));
      std::vector<bool> result;
      std::string part;
      bool flag;
      for (auto &ch : data) {
        if (flag && ch == ',') {
          std::stringstream ss(part);
          bool val;
          ss >> val;
          result.push_back(val);
          part.clear();
        } else {
          part += ch;
        }
      }
      std::stringstream ss(part);
      bool val;
      ss >> val;
      result.push_back(val);
      return result;
    } else {
      return {};
    }
  }
  throw std::runtime_error(fmt::format("Unknown field '{}'", name));
}
void Record::setField(const std::string &name, const std::string &source) {
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
  std::string result;
  for (auto i = 0; i < source.size(); i++) {
    if (i != 0) {
      result += ",";
    }
    result += "\"";
    for (auto &ch : source[i]) {
      if (ch == '\"') {
        result += "\\\"";
      } else {
        result += ch;
      }
    }
    result += "\"";
  }
  _record[name] = source;
}
void Record::setField(const std::string &name,
                      const std::vector<bool> &source) {
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
  std::string result;
  for (auto i = 0; i < source.size(); i++) {
    if (i != 0) {
      result += ",";
    }
    result += std::to_string(source[i]);
  }
  _record[name] = source;
}
void Record::setField(const std::string &name,
                      const std::vector<int32_t> &source) {
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
  std::string result;
  for (auto i = 0; i < source.size(); i++) {
    if (i != 0) {
      result += ",";
    }
    result += std::to_string(source[i]);
  }
  _record[name] = source;
}
void Record::setField(const std::string &name,
                      const std::vector<double> &source) {
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
  std::string result;
  for (auto i = 0; i < source.size(); i++) {
    if (i != 0) {
      result += ",";
    }
    result += std::to_string(source[i]);
  }
  _record[name] = source;
}