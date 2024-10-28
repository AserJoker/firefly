#pragma once
#include "Any.hpp"
#include "Array.hpp"
#include "BaseValue.hpp"
#include "CompileString.hpp"
#include "Map.hpp"
#include "core/CompileString.hpp"
#include <cjson/cJSON.h>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <stdexcept>
#include <string>


namespace firefly::core {
class Value;
using Nil_t = std::nullptr_t;
using Byte_t = uint8_t;
using Boolean_t = bool;
using String_t = std::string;
using CString_t = const char *;
using Integer_t = int32_t;
using Unsigned_t = uint32_t;
using Float_t = float;
using Array_t = core::Array<Value>;
using Map_t = core::Map<String_t, Value>;

enum class ValueType {
  NIL = 0,
  BYTE,
  BOOLEAN,
  STRING,
  INTEGER,
  UNSIGNED,
  FLOAT,
  ARRAY,
  MAP
};
template <class T, auto v, CompileString n, class V = T>
struct TypeEx : Type<T, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    throw std::runtime_error(
        fmt::format("Cannot convert '{}' to '{}'", stype, n.value));
  }
};
template <auto v, CompileString n>
struct TypeEx<Nil_t, v, n> : Type<Nil_t, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    return nullptr;
  }
};

template <auto v, CompileString n>
struct TypeEx<Byte_t, v, n> : Type<Byte_t, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    switch (type) {
    case ValueType::NIL:
      return Byte_t(0);
    case ValueType::BYTE:
      return value;
    case ValueType::BOOLEAN:
      return Byte_t(value.as<Boolean_t>() ? 1 : 0);
    case ValueType::STRING:
      return (Byte_t)(std::stoi(value.as<String_t>(), NULL, 0));
    case ValueType::INTEGER:
      return (Byte_t)value.as<Integer_t>();
    case ValueType::UNSIGNED:
      return (Byte_t)value.as<Unsigned_t>();
    case ValueType::FLOAT:
      return (Byte_t)value.as<Float_t>();
    default:
      break;
    }
    throw std::runtime_error(
        fmt::format("Cannot convert '{}' to '{}'", stype, n.value));
  }
};

template <auto v, CompileString n>
struct TypeEx<Boolean_t, v, n> : Type<Boolean_t, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    switch (type) {
    case ValueType::NIL:
      return false;
    case ValueType::BYTE:
      return value.as<Byte_t>() == 1;
    case ValueType::BOOLEAN:
      return value;
    case ValueType::STRING:
      return value.as<String_t>() != "false";
    case ValueType::INTEGER:
      return value.as<Integer_t>() == 1;
    case ValueType::UNSIGNED:
      return value.as<Unsigned_t>() == 1;
    case ValueType::FLOAT:
      return value.as<Float_t>() == 1;
    default:
      break;
    }
    throw std::runtime_error(
        fmt::format("Cannot convert '{}' to '{}'", stype, n.value));
  }
};

template <auto v, CompileString n>
struct TypeEx<String_t, v, n> : Type<String_t, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    switch (type) {
    case ValueType::NIL:
      return nullptr;
    case ValueType::BYTE:
      return fmt::format("{:d}", (Unsigned_t)value.as<Byte_t>());
    case ValueType::BOOLEAN:
      return value.as<Boolean_t>() ? "true" : "false";
    case ValueType::STRING:
      return value;
    case ValueType::INTEGER:
      return fmt::format("{:d}", value.as<Integer_t>());
    case ValueType::UNSIGNED:
      return fmt::format("{:d}", value.as<Unsigned_t>());
    case ValueType::FLOAT:
      return fmt::format("{:g}", value.as<Float_t>());
    default:
      break;
    }
    throw std::runtime_error(
        fmt::format("Cannot convert '{}' to '{}'", stype, n.value));
  }
};
template <auto v, CompileString n>
struct TypeEx<Integer_t, v, n> : Type<Integer_t, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    switch (type) {
    case ValueType::NIL:
      return Integer_t(0);
    case ValueType::BYTE:
      return Integer_t(value.as<Byte_t>());
    case ValueType::BOOLEAN:
      return Integer_t(value.as<Boolean_t>() ? 1 : 0);
    case ValueType::STRING:
      return Integer_t(std::stoi(value.as<String_t>(), NULL, 0));
    case ValueType::INTEGER:
      return value;
    case ValueType::UNSIGNED:
      return (Integer_t)value.as<Unsigned_t>();
    case ValueType::FLOAT:
      return (Integer_t)value.as<Float_t>();
    default:
      break;
    }
    throw std::runtime_error(
        fmt::format("Cannot convert '{}' to '{}'", stype, n.value));
  }
};
template <auto v, CompileString n>
struct TypeEx<Unsigned_t, v, n> : Type<Unsigned_t, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    switch (type) {
    case ValueType::NIL:
      return Unsigned_t(0);
    case ValueType::BYTE:
      return Unsigned_t(value.as<Byte_t>());
    case ValueType::BOOLEAN:
      return Unsigned_t(value.as<Boolean_t>() ? 1 : 0);
    case ValueType::STRING:
      return Unsigned_t(std::stoi(value.as<String_t>(), NULL, 0));
    case ValueType::INTEGER:
      return (Unsigned_t)value.as<Integer_t>();
    case ValueType::UNSIGNED:
      return value;
    case ValueType::FLOAT:
      return (Unsigned_t)value.as<Float_t>();
    default:
      break;
    }
    throw std::runtime_error(
        fmt::format("Cannot convert '{}' to '{}'", stype, n.value));
  }
};

template <auto v, CompileString n>
struct TypeEx<Float_t, v, n> : Type<Float_t, v, n> {
  static Any as(const Any &value, const ValueType &type,
                const std::string &stype) {
    switch (type) {
    case ValueType::NIL:
      return Float_t(0);
    case ValueType::BYTE:
      return Float_t(value.as<Byte_t>());
    case ValueType::BOOLEAN:
      return Float_t(value.as<Boolean_t>() ? 1 : 0);
    case ValueType::STRING:
      return Float_t(std::stof(value.as<String_t>()));
    case ValueType::INTEGER:
      return (Float_t)value.as<Integer_t>();
    case ValueType::UNSIGNED:
      return (Float_t)value.as<Unsigned_t>();
    case ValueType::FLOAT:
      return value;
    default:
      break;
    }
    throw std::runtime_error(
        fmt::format("Cannot convert '{}' to '{}'", stype, n.value));
  }
};

using NilType = TypeEx<Nil_t, ValueType::NIL, "nil">;
using ByteType = TypeEx<Byte_t, ValueType::BYTE, "byte">;
using BooleanType = TypeEx<Boolean_t, ValueType::BOOLEAN, "boolean">;
using StringType = TypeEx<String_t, ValueType::STRING, "string">;
using IntegerType = TypeEx<Integer_t, ValueType::INTEGER, "integer">;
using UnsignedType = TypeEx<Unsigned_t, ValueType::UNSIGNED, "unsigned">;
using FloatType = TypeEx<Float_t, ValueType::FLOAT, "float">;
using ArrayType = TypeEx<Array_t, ValueType::ARRAY, "array">;
using MapType = TypeEx<Map_t, ValueType::MAP, "map">;

using TypedBaseValue =
    BaseValue<NilType, ByteType, BooleanType, StringType, IntegerType,
              UnsignedType, FloatType, ArrayType, MapType>;

class Value : public TypedBaseValue {
public:
  Value() : TypedBaseValue(nullptr) {}
  template <class T> Value(T value) : TypedBaseValue(value) {}
  Value(CString_t value) : Value(std::string(value)) {}
  template <class T> Value as() const {
    return Select<T>::as(_value, _type, _typename).template as<T>();
  }
  template <class T> T to() const { return as<T>().template get<T>(); }
};

} // namespace firefly::core
