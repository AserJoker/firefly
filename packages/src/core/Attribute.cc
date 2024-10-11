#include "core/Attribute.hpp"
#include <algorithm>
#include <any>
#include <cctype>
#include <sstream>
using namespace firefly::core;

PtrAttribute::PtrAttribute() {
  type = AttributeType::NIL;
  i32 = nullptr;
}
PtrAttribute::PtrAttribute(std::nullptr_t) {
  type = AttributeType::NIL;
  i32 = nullptr;
}
PtrAttribute::PtrAttribute(bool *boolean) {
  type = AttributeType::BOOLEAN;
  this->boolean = boolean;
}
PtrAttribute::PtrAttribute(int32_t *i32) {
  type = AttributeType::I32;
  this->i32 = i32;
}
PtrAttribute::PtrAttribute(uint32_t *u32) {
  type = AttributeType::U32;
  this->u32 = u32;
}
PtrAttribute::PtrAttribute(float *f32) {
  type = AttributeType::F32;
  this->f32 = f32;
}
PtrAttribute::PtrAttribute(std::string *string) {
  type = AttributeType::STRING;
  this->string = string;
}
PtrAttribute &PtrAttribute::operator=(const Attribute &attr) {
  switch (type) {
  case AttributeType::NIL:
    break;
  case AttributeType::BOOLEAN:
    *boolean = attr.toBoolean();
    break;
  case AttributeType::I32:
    *i32 = attr.toInt32();
    break;
  case AttributeType::U32:
    *u32 = attr.toUint32();
    break;
  case AttributeType::F32:
    *f32 = attr.toFloat32();
    break;
  case AttributeType::STRING:
    *string = attr.toString();
    break;
  }
  return *this;
}
bool PtrAttribute::operator==(const Attribute &attr) {
  switch (type) {
  case AttributeType::NIL:
    return attr.getType() == AttributeType::NIL;
  case AttributeType::BOOLEAN:
    return *boolean == attr.toBoolean();
  case AttributeType::I32:
    return *i32 == attr.toInt32();
  case AttributeType::U32:
    return *u32 == attr.toUint32();
  case AttributeType::F32:
    return *f32 == attr.toFloat32();
  case AttributeType::STRING:
    return *string == attr.toString();
  }
  return false;
}
bool PtrAttribute::operator==(const PtrAttribute &attr) {
  return type == attr.type && *this == attr.toAttribute();
}
Attribute PtrAttribute::toAttribute() const {
  switch (type) {
  case AttributeType::NIL:
    return nullptr;
  case AttributeType::BOOLEAN:
    return *boolean;
  case AttributeType::I32:
    return *i32;
  case AttributeType::U32:
    return *u32;
  case AttributeType::F32:
    return *f32;
  case AttributeType::STRING:
    return *string;
  }
  return {};
}

Attribute::Attribute() { _type = AttributeType::NIL; }

Attribute::Attribute(const Attribute &another) {
  _type = another._type;
  switch (_type) {
  case AttributeType::NIL:
    _value = nullptr;
    break;
  case AttributeType::BOOLEAN:
    _value = std::any_cast<bool>(another._value);
    break;
  case AttributeType::I32:
    _value = std::any_cast<int32_t>(another._value);
    break;
  case AttributeType::U32:
    _value = std::any_cast<uint32_t>(another._value);
    break;
  case AttributeType::F32:
    _value = std::any_cast<float>(another._value);
    break;
  case AttributeType::STRING:
    _value = std::any_cast<std::string>(another._value);
    break;
  }
}
Attribute::Attribute(std::nullptr_t) { _type = AttributeType::NIL; }

Attribute::Attribute(bool boolean) {
  _type = AttributeType::BOOLEAN;
  _value = boolean;
}
Attribute::Attribute(int32_t i32) {
  _type = AttributeType::I32;
  _value = i32;
}
Attribute::Attribute(uint32_t u32) {
  _type = AttributeType::U32;
  _value = u32;
}
Attribute::Attribute(float f32) {
  _type = AttributeType::F32;
  _value = f32;
}
Attribute::Attribute(const std::string &string) {
  _type = AttributeType::STRING;
  _value = string;
}
const AttributeType &Attribute::getType() const { return _type; }
Attribute &Attribute::operator=(std::nullptr_t) {
  _type = AttributeType::NIL;
  _value = nullptr;
  return *this;
}
Attribute &Attribute::operator=(bool boolean) {
  _type = AttributeType::BOOLEAN;
  _value = boolean;
  return *this;
}
Attribute &Attribute::operator=(int32_t i32) {
  _type = AttributeType::I32;
  _value = i32;
  return *this;
}
Attribute &Attribute::operator=(uint32_t u32) {
  _type = AttributeType::U32;
  _value = u32;
  return *this;
}
Attribute &Attribute::operator=(float f32) {
  _type = AttributeType::F32;
  _value = f32;
  return *this;
}
Attribute &Attribute::operator=(const std::string &string) {
  _type = AttributeType::STRING;
  _value = string;
  return *this;
}
Attribute &Attribute::operator=(const Attribute &another) {
  _type = another._type;
  switch (_type) {
  case AttributeType::NIL:
    _value = nullptr;
    break;
  case AttributeType::BOOLEAN:
    _value = std::any_cast<bool>(another._value);
    break;
  case AttributeType::I32:
    _value = std::any_cast<int32_t>(another._value);
    break;
  case AttributeType::U32:
    _value = std::any_cast<uint32_t>(another._value);
    break;
  case AttributeType::F32:
    _value = std::any_cast<float>(another._value);
    break;
  case AttributeType::STRING:
    _value = std::any_cast<std::string>(another._value);
    break;
  }
  return *this;
}
bool Attribute::operator==(std::nullptr_t) {
  return _type == AttributeType::NIL;
}
bool Attribute::operator==(bool boolean) { return toBoolean() == boolean; }
bool Attribute::operator==(int32_t i32) { return toInt32() == i32; }
bool Attribute::operator==(uint32_t u32) { return toUint32() == u32; }
bool Attribute::operator==(float f32) { return toFloat32() == f32; }
bool Attribute::operator==(const std::string &string) {
  return toString() == string;
}
bool Attribute::operator==(const Attribute &another) {
  if (_type != another._type) {
    return false;
  }
  switch (_type) {
  case AttributeType::NIL:
    return true;
  case AttributeType::BOOLEAN:
    return toBoolean() == another.toBoolean();
  case AttributeType::I32:
    return toInt32() == another.toInt32();
  case AttributeType::U32:
    return toUint32() == another.toUint32();
  case AttributeType::F32:
    return toFloat32() == another.toFloat32();
  case AttributeType::STRING:
    return toString() == another.toString();
  }
  return false;
}

std::string Attribute::toString() const {
  switch (_type) {
  case AttributeType::NIL:
    return "nil";
  case AttributeType::BOOLEAN:
    return std::any_cast<bool>(_value) ? "true" : "false";
  case AttributeType::I32:
    return fmt::format("{}", std::any_cast<int32_t>(_value));
  case AttributeType::U32:
    return fmt::format("{}", std::any_cast<uint32_t>(_value));
  case AttributeType::F32:
    return fmt::format("{:g}", std::any_cast<float>(_value));
  case AttributeType::STRING:
    return std::any_cast<std::string>(_value);
  }
  return "";
}

bool Attribute::toBoolean() const {
  switch (_type) {
  case AttributeType::NIL:
    return false;
  case AttributeType::BOOLEAN:
    return std::any_cast<bool>(_value);
  case AttributeType::I32:
    return std::any_cast<int32_t>(_value) == 1;
  case AttributeType::U32:
    return std::any_cast<uint32_t>(_value) == 1;
  case AttributeType::F32:
    return std::any_cast<float>(_value) == 1;
  case AttributeType::STRING: {
    auto string = std::any_cast<std::string>(_value);
    std::transform(string.begin(), string.end(), string.begin(), tolower);
    return string != "false";
  }
  }
  return false;
}

int32_t Attribute::toInt32() const {
  switch (_type) {

  case AttributeType::NIL:
    return 0;
  case AttributeType::BOOLEAN:
    return std::any_cast<bool>(_value) ? 1 : 0;
  case AttributeType::I32:
    return std::any_cast<int32_t>(_value);
  case AttributeType::U32:
    return (int32_t)std::any_cast<uint32_t>(_value);
  case AttributeType::F32:
    return (int32_t)std::any_cast<float>(_value);
  case AttributeType::STRING: {
    std::stringstream ss(std::any_cast<std::string>(_value));
    int32_t value = 0;
    ss >> value;
    return value;
  }
  }
  return 0;
}

uint32_t Attribute::toUint32() const {
  switch (_type) {

  case AttributeType::NIL:
    return 0;
  case AttributeType::BOOLEAN:
    return std::any_cast<bool>(_value) ? 1 : 0;
  case AttributeType::I32:
    return (uint32_t)std::any_cast<int32_t>(_value);
  case AttributeType::U32:
    return std::any_cast<uint32_t>(_value);
  case AttributeType::F32:
    return (uint32_t)std::any_cast<float>(_value);
  case AttributeType::STRING: {
    std::stringstream ss(std::any_cast<std::string>(_value));
    uint32_t value = 0;
    ss >> value;
    return value;
  }
  }
  return 0;
}

float Attribute::toFloat32() const {
  switch (_type) {
  case AttributeType::NIL:
    return 0.f;
  case AttributeType::BOOLEAN:
    return std::any_cast<bool>(_value) ? 1.f : 0.f;
  case AttributeType::I32:
    return (float)std::any_cast<int32_t>(_value);
  case AttributeType::U32:
    return (float)std::any_cast<uint32_t>(_value);
  case AttributeType::F32:
    return std::any_cast<float>(_value);
  case AttributeType::STRING: {
    std::stringstream ss(std::any_cast<std::string>(_value));
    float value = 0;
    ss >> value;
    return value;
  }
  }
  return 0.f;
}