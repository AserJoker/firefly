#include "database/Value.hpp"
#include <any>
#include <cstddef>
#include <fmt/core.h>
#include <stdexcept>
#include <vector>
using namespace firefly;
using namespace firefly::database;
Value::Value(std::any &&value) : _value(std::move(value)) {}
const std::any &Value::getValue() const { return _value; }

void Value::setValue(std::any &&value) { _value = std::move(value); }
const bool Value::isEqual(const core::AutoPtr<Value> &another) const {
  if (_value.type() != another->_value.type()) {
    return false;
  }
  if (_value.type() == typeid(std::string)) {
    return std::any_cast<std::string>(_value) ==
           std::any_cast<std::string>(another->_value);
  }
  if (_value.type() == typeid(double)) {
    return std::any_cast<double>(_value) ==
           std::any_cast<double>(another->_value);
  }
  if (_value.type() == typeid(int32_t)) {
    return std::any_cast<int32_t>(_value) ==
           std::any_cast<int32_t>(another->_value);
  }
  if (_value.type() == typeid(bool)) {
    return std::any_cast<bool>(_value) == std::any_cast<bool>(another->_value);
  }

  if (_value.type() == typeid(std::vector<std::string>)) {
    return std::any_cast<std::vector<std::string>>(_value) ==
           std::any_cast<std::vector<std::string>>(another->_value);
  }
  if (_value.type() == typeid(std::vector<double>)) {
    return std::any_cast<std::vector<double>>(_value) ==
           std::any_cast<std::vector<double>>(another->_value);
  }
  if (_value.type() == typeid(std::vector<int32_t>)) {
    return std::any_cast<std::vector<int32_t>>(_value) ==
           std::any_cast<std::vector<int32_t>>(another->_value);
  }
  if (_value.type() == typeid(std::vector<bool>)) {
    return std::any_cast<std::vector<bool>>(_value) ==
           std::any_cast<std::vector<bool>>(another->_value);
  }
  if (_value.type() == typeid(std::nullptr_t)) {
    return true;
  }
  return false;
}
const std::string Value::toString() const {
  if (_value.type() == typeid(std::string)) {
    return std::any_cast<std::string>(_value);
  }
  if (_value.type() == typeid(double)) {
    return fmt::format("{}", std::any_cast<double>(_value));
  }
  if (_value.type() == typeid(uint32_t)) {
    return fmt::format("{}", std::any_cast<int32_t>(_value));
  }
  if (_value.type() == typeid(bool)) {
    return fmt::format("{}", std::any_cast<bool>(_value));
  }
  if (_value.type() == typeid(std::vector<std::string>)) {
    auto array = std::any_cast<std::vector<std::string>>(_value);
    std::string result = "";
    for (auto &item : array) {
      if (!result.empty()) {
        result += ",";
      }
      result += item;
    }
    return result;
  }
  if (_value.type() == typeid(std::vector<double>)) {
    auto array = std::any_cast<std::vector<double>>(_value);
    std::string result = "";
    for (auto &item : array) {
      if (!result.empty()) {
        result += ",";
      }
      result += fmt::format("{}", item);
    }
    return result;
  }
  if (_value.type() == typeid(std::vector<int32_t>)) {
    auto array = std::any_cast<std::vector<int32_t>>(_value);
    std::string result = "";
    for (auto &item : array) {
      if (!result.empty()) {
        result += ",";
      }
      result += fmt::format("{}", item);
    }
    return result;
  }
  if (_value.type() == typeid(std::vector<bool>)) {
    auto array = std::any_cast<std::vector<bool>>(_value);
    std::string result = "";
    for (auto item : array) {
      if (!result.empty()) {
        result += ",";
      }
      result += fmt::format("{}", item ? "true" : "false");
    }
    return result;
  }
  if (_value.type() == typeid(std::nullptr_t)) {
    return "nil";
  }
  return "";
}

const std::string Value::getStringValue() const {
  if (_value.type() != typeid(std::string)) {
    throw std::runtime_error("Failed to read string value");
  }
  return std::any_cast<std::string>(_value);
}
const int32_t Value::getIntegerValue() const {
  if (_value.type() != typeid(int32_t)) {
    throw std::runtime_error("Failed to read integer value");
  }
  return std::any_cast<int32_t>(_value);
}
const double Value::getNumberValue() const {
  if (_value.type() != typeid(double)) {
    throw std::runtime_error("Failed to read number value");
  }
  return std::any_cast<double>(_value);
}
const bool Value::getBooleanValue() const {
  if (_value.type() != typeid(bool)) {
    throw std::runtime_error("Failed to read boolean value");
  }
  return std::any_cast<bool>(_value);
}

const std::vector<std::string> Value::getStringArrayValue() const {
  if (_value.type() != typeid(std::vector<std::string>)) {
    throw std::runtime_error("Failed to read string array value");
  }
  return std::any_cast<std::vector<std::string>>(_value);
}
const std::vector<int32_t> Value::getIntegerArrayValue() const {
  if (_value.type() != typeid(std::vector<int32_t>)) {
    throw std::runtime_error("Failed to read integer array value");
  }
  return std::any_cast<std::vector<int32_t>>(_value);
}
const std::vector<double> Value::getNumberArrayValue() const {
  if (_value.type() != typeid(std::vector<double>)) {
    throw std::runtime_error("Failed to read number array value");
  }
  return std::any_cast<std::vector<double>>(_value);
}
const std::vector<bool> Value::getBooleanArrayValue() const {
  if (_value.type() != typeid(std::vector<bool>)) {
    throw std::runtime_error("Failed to read boolean array value");
  }
  return std::any_cast<std::vector<bool>>(_value);
}
const bool Value::isNil() const {
  return _value.type() == typeid(std::nullptr_t);
}
const bool Value::isString() const {
  return _value.type() == typeid(std::string) ||
         _value.type() == typeid(std::vector<std::string>);
}
const bool Value::isNumber() const {
  return _value.type() == typeid(double) ||
         _value.type() == typeid(std::vector<double>);
}
const bool Value::isInteger() const {
  return _value.type() == typeid(int32_t) ||
         _value.type() == typeid(std::vector<int32_t>);
}
const bool Value::isBoolean() const {
  return _value.type() == typeid(bool) ||
         _value.type() == typeid(std::vector<bool>);
}