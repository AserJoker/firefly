#pragma once
#include "Array.hpp"
#include "BaseValue.hpp"
#include "Map.hpp"
#include "TemplateCString.hpp"
#include <cjson/cJSON.h>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <sstream>
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
  static std::string toString(const T &value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }
  static Any as(const Any &value, const ValueType &type) {
    throw std::runtime_error("not support convert");
  }
};

template <>
struct TypeEx<Nil_t, ValueType::NIL, "nil">
    : Type<Nil_t, ValueType::NIL, "nil"> {
  static std::string toString(const std::nullptr_t &value) { return "nullptr"; }
  static Any as(const Any &value, const ValueType &type) { return nullptr; }
};

template <>
struct TypeEx<String_t, ValueType::STRING, "string">
    : Type<String_t, ValueType::STRING, "string"> {
  static std::string toString(const String_t &value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }
  static Any as(const Any &value, const ValueType &type) {
    switch (type) {
    case ValueType::NIL:
      return String_t("nullptr");
    case ValueType::BYTE:
      return std::to_string(value.as<Byte_t>());
    case ValueType::BOOLEAN:
      return std::to_string(value.as<Boolean_t>());
    case ValueType::STRING:
      return value.as<String_t>();
    case ValueType::INTEGER:
      return std::to_string(value.as<Integer_t>());
    case ValueType::UNSIGNED:
      return std::to_string(value.as<Unsigned_t>());
    case ValueType::FLOAT:
      return std::to_string(value.as<Float_t>());
    case ValueType::ARRAY:
    case ValueType::MAP:
      break;
    }
    return String_t("[Invalid value]");
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
private:
  template <class T> std::string toString() const {
    return Select<T>::toString(get<T>());
  }

public:
  template <class T> Value(T value) : TypedBaseValue(value) {}
  Value(CString_t value) : Value(std::string(value)) {}
  std::string toString() const {
    switch (_type) {
    case ValueType::NIL:
      return "nil";
    case ValueType::BYTE:
      return toString<Byte_t>();
    case ValueType::BOOLEAN:
      return toString<Boolean_t>();
    case ValueType::STRING:
      return toString<String_t>();
    case ValueType::INTEGER:
      return toString<Integer_t>();
    case ValueType::UNSIGNED:
      return toString<Unsigned_t>();
    case ValueType::FLOAT:
      return toString<Float_t>();
    case ValueType::ARRAY:
      return toString<Array_t>();
    case ValueType::MAP:
      return toString<Map_t>();
    }
    return "[Invalid value]";
  }
  cJSON *toJSON() const {
    switch (_type) {
    case ValueType::NIL:
      return cJSON_CreateNull();
    case ValueType::BYTE:
      return cJSON_CreateNumber(get<Byte_t>());
    case ValueType::BOOLEAN:
      return cJSON_CreateBool(get<Boolean_t>());
    case ValueType::STRING:
      return cJSON_CreateString(get<String_t>().c_str());
    case ValueType::INTEGER:
      return cJSON_CreateNumber(get<Integer_t>());
    case ValueType::UNSIGNED:
      return cJSON_CreateNumber(get<Unsigned_t>());
    case ValueType::FLOAT:
      return cJSON_CreateNumber(get<Float_t>());
    case ValueType::ARRAY: {
      cJSON *node = cJSON_CreateArray();
      auto array = get<Array_t>();
      for (auto &item : array) {
        cJSON_AddItemToArray(node, item.toJSON());
      }
      return node;
    }
    case ValueType::MAP: {
      cJSON *node = cJSON_CreateObject();
      auto map = get<Map_t>();
      for (auto &[k, v] : map) {
        cJSON_AddItemToObject(node, k.c_str(), v.toJSON());
      }
      return node;
    }
    }
    return nullptr;
  }
  template <class T> Value as() {
    return Select<T>::as(_value, _type).template as<T>();
  }
};

static inline std::ostream &operator<<(std::ostream &out, const Value &value) {
  out << value.toString();
  return out;
}

template <class T>
static inline std::ostream &operator<<(std::ostream &out,
                                       const Array<T> &value) {
  out << "[]";
  return out;
}

template <class T, class K>
static inline std::ostream &operator<<(std::ostream &out,
                                       const Map<T, K> &value) {
  out << "{}";
  return out;
}
} // namespace firefly::core
