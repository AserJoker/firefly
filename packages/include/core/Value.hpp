#pragma once
#include "Array.hpp"
#include "BaseValue.hpp"
#include "Map.hpp"
#include "TemplateCString.hpp"
#include <cjson/cJSON.h>
#include <cstddef>
#include <cstdint>
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
  static Any as(const Any &value, const ValueType &type) {
    throw std::runtime_error("not support convert");
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
  template <class T> Value(T value) : TypedBaseValue(value) {}
  Value(CString_t value) : Value(std::string(value)) {}
  template <class T> Value as() {
    return Select<T>::as(_value, _type).template as<T>();
  }
};

} // namespace firefly::core
