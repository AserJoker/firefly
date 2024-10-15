#include "core/Value.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
using namespace firefly::core;

ValuePtr::ValuePtr() {
  type = Value::Type::NIL;
  i32 = nullptr;
}
ValuePtr::ValuePtr(std::nullptr_t) {
  type = Value::Type::NIL;
  i32 = nullptr;
}
ValuePtr::ValuePtr(bool *boolean) {
  type = Value::Type::BOOLEAN;
  this->boolean = boolean;
}
ValuePtr::ValuePtr(int32_t *i32) {
  type = Value::Type::I32;
  this->i32 = i32;
}
ValuePtr::ValuePtr(uint32_t *u32) {
  type = Value::Type::U32;
  this->u32 = u32;
}
ValuePtr::ValuePtr(float *f32) {
  type = Value::Type::F32;
  this->f32 = f32;
}
ValuePtr::ValuePtr(std::string *string) {
  type = Value::Type::STRING;
  this->string = string;
}
ValuePtr &ValuePtr::operator=(const Value &attr) {
  switch (type) {
  case Value::Type::NIL:
    break;
  case Value::Type::BOOLEAN:
    *boolean = attr.toBoolean();
    break;
  case Value::Type::I32:
    *i32 = attr.toInt32();
    break;
  case Value::Type::U32:
    *u32 = attr.toUint32();
    break;
  case Value::Type::F32:
    *f32 = attr.toFloat32();
    break;
  case Value::Type::STRING:
    *string = attr.toString();
    break;
  }
  return *this;
}
bool ValuePtr::operator==(const Value &attr) {
  switch (type) {
  case Value::Type::NIL:
    return attr.getType() == Value::Type::NIL;
  case Value::Type::BOOLEAN:
    return *boolean == attr.toBoolean();
  case Value::Type::I32:
    return *i32 == attr.toInt32();
  case Value::Type::U32:
    return *u32 == attr.toUint32();
  case Value::Type::F32:
    return *f32 == attr.toFloat32();
  case Value::Type::STRING:
    return *string == attr.toString();
  }
  return false;
}
bool ValuePtr::operator==(const ValuePtr &attr) {
  return type == attr.type && *this == attr.toValue();
}
Value ValuePtr::toValue() const {
  switch (type) {
  case Value::Type::NIL:
    return nullptr;
  case Value::Type::BOOLEAN:
    return *boolean;
  case Value::Type::I32:
    return *i32;
  case Value::Type::U32:
    return *u32;
  case Value::Type::F32:
    return *f32;
  case Value::Type::STRING:
    return *string;
  }
  return {};
}
std::string ValuePtr::getTypeName() const {
  switch (type) {

  case Value::Type::NIL:
    return "nil";
  case Value::Type::BOOLEAN:
    return "boolean";
  case Value::Type::I32:
    return "int32";
  case Value::Type::U32:
    return "uint32";
  case Value::Type::F32:
    return "float32";
  case Value::Type::STRING:
    return "string";
  }
  return "unknown";
}

Value::Value() { _type = Value::Type::NIL; }

Value::Value(const Value &another) {
  _type = another._type;
  switch (_type) {
  case Value::Type::NIL:
    _value = nullptr;
    break;
  case Value::Type::BOOLEAN:
    _value = std::any_cast<bool>(another._value);
    break;
  case Value::Type::I32:
    _value = std::any_cast<int32_t>(another._value);
    break;
  case Value::Type::U32:
    _value = std::any_cast<uint32_t>(another._value);
    break;
  case Value::Type::F32:
    _value = std::any_cast<float>(another._value);
    break;
  case Value::Type::STRING:
    _value = std::any_cast<std::string>(another._value);
    break;
  }
}
Value::Value(std::nullptr_t) { _type = Value::Type::NIL; }

Value::Value(bool boolean) {
  _type = Value::Type::BOOLEAN;
  _value = boolean;
}
Value::Value(int32_t i32) {
  _type = Value::Type::I32;
  _value = i32;
}
Value::Value(uint32_t u32) {
  _type = Value::Type::U32;
  _value = u32;
}
Value::Value(float f32) {
  _type = Value::Type::F32;
  _value = f32;
}
Value::Value(const std::string &string) {
  _type = Value::Type::STRING;
  _value = string;
}
const Value::Type &Value::getType() const { return _type; }
Value &Value::operator=(std::nullptr_t) {
  _type = Value::Type::NIL;
  _value = nullptr;
  return *this;
}
Value &Value::operator=(bool boolean) {
  _type = Value::Type::BOOLEAN;
  _value = boolean;
  return *this;
}
Value &Value::operator=(int32_t i32) {
  _type = Value::Type::I32;
  _value = i32;
  return *this;
}
Value &Value::operator=(uint32_t u32) {
  _type = Value::Type::U32;
  _value = u32;
  return *this;
}
Value &Value::operator=(float f32) {
  _type = Value::Type::F32;
  _value = f32;
  return *this;
}
Value &Value::operator=(const std::string &string) {
  _type = Value::Type::STRING;
  _value = string;
  return *this;
}
Value &Value::operator=(const Value &another) {
  _type = another._type;
  switch (_type) {
  case Value::Type::NIL:
    _value = nullptr;
    break;
  case Value::Type::BOOLEAN:
    _value = std::any_cast<bool>(another._value);
    break;
  case Value::Type::I32:
    _value = std::any_cast<int32_t>(another._value);
    break;
  case Value::Type::U32:
    _value = std::any_cast<uint32_t>(another._value);
    break;
  case Value::Type::F32:
    _value = std::any_cast<float>(another._value);
    break;
  case Value::Type::STRING:
    _value = std::any_cast<std::string>(another._value);
    break;
  }
  return *this;
}
bool Value::operator==(std::nullptr_t) {
  return _type == Value::Type::NIL;
}
bool Value::operator==(bool boolean) { return toBoolean() == boolean; }
bool Value::operator==(int32_t i32) { return toInt32() == i32; }
bool Value::operator==(uint32_t u32) { return toUint32() == u32; }
bool Value::operator==(float f32) { return toFloat32() == f32; }
bool Value::operator==(const std::string &string) {
  return toString() == string;
}
bool Value::operator==(const Value &another) {
  if (_type != another._type) {
    return false;
  }
  switch (_type) {
  case Value::Type::NIL:
    return true;
  case Value::Type::BOOLEAN:
    return toBoolean() == another.toBoolean();
  case Value::Type::I32:
    return toInt32() == another.toInt32();
  case Value::Type::U32:
    return toUint32() == another.toUint32();
  case Value::Type::F32:
    return toFloat32() == another.toFloat32();
  case Value::Type::STRING:
    return toString() == another.toString();
  }
  return false;
}

std::string Value::toString() const {
  switch (_type) {
  case Value::Type::NIL:
    return "nil";
  case Value::Type::BOOLEAN:
    return std::any_cast<bool>(_value) ? "true" : "false";
  case Value::Type::I32:
    return fmt::format("{}", std::any_cast<int32_t>(_value));
  case Value::Type::U32:
    return fmt::format("{}", std::any_cast<uint32_t>(_value));
  case Value::Type::F32:
    return fmt::format("{:g}", std::any_cast<float>(_value));
  case Value::Type::STRING:
    return std::any_cast<std::string>(_value);
  }
  return "";
}

bool Value::toBoolean() const {
  switch (_type) {
  case Value::Type::NIL:
    return false;
  case Value::Type::BOOLEAN:
    return std::any_cast<bool>(_value);
  case Value::Type::I32:
    return std::any_cast<int32_t>(_value) == 1;
  case Value::Type::U32:
    return std::any_cast<uint32_t>(_value) == 1;
  case Value::Type::F32:
    return std::any_cast<float>(_value) == 1;
  case Value::Type::STRING: {
    auto string = std::any_cast<std::string>(_value);
    std::transform(string.begin(), string.end(), string.begin(), tolower);
    return string != "false";
  }
  }
  return false;
}

int32_t Value::toInt32() const {
  switch (_type) {

  case Value::Type::NIL:
    return 0;
  case Value::Type::BOOLEAN:
    return std::any_cast<bool>(_value) ? 1 : 0;
  case Value::Type::I32:
    return std::any_cast<int32_t>(_value);
  case Value::Type::U32:
    return (int32_t)std::any_cast<uint32_t>(_value);
  case Value::Type::F32:
    return (int32_t)std::any_cast<float>(_value);
  case Value::Type::STRING: {
    std::stringstream ss(std::any_cast<std::string>(_value));
    int32_t value = 0;
    ss >> value;
    return value;
  }
  }
  return 0;
}

uint32_t Value::toUint32() const {
  switch (_type) {

  case Value::Type::NIL:
    return 0;
  case Value::Type::BOOLEAN:
    return std::any_cast<bool>(_value) ? 1 : 0;
  case Value::Type::I32:
    return (uint32_t)std::any_cast<int32_t>(_value);
  case Value::Type::U32:
    return std::any_cast<uint32_t>(_value);
  case Value::Type::F32:
    return (uint32_t)std::any_cast<float>(_value);
  case Value::Type::STRING: {
    std::stringstream ss(std::any_cast<std::string>(_value));
    uint32_t value = 0;
    ss >> value;
    return value;
  }
  }
  return 0;
}

float Value::toFloat32() const {
  switch (_type) {
  case Value::Type::NIL:
    return 0.f;
  case Value::Type::BOOLEAN:
    return std::any_cast<bool>(_value) ? 1.f : 0.f;
  case Value::Type::I32:
    return (float)std::any_cast<int32_t>(_value);
  case Value::Type::U32:
    return (float)std::any_cast<uint32_t>(_value);
  case Value::Type::F32:
    return std::any_cast<float>(_value);
  case Value::Type::STRING: {
    std::stringstream ss(std::any_cast<std::string>(_value));
    float value = 0;
    ss >> value;
    return value;
  }
  }
  return 0.f;
}