#pragma once
#include "Array.hpp"
#include "Map.hpp"
#include <any>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <typeinfo>

namespace firefly::core {
class Value {
public:
  enum class TYPE {
    NIL = 0,
    BYTE,
    INTEGER,
    UNSIGNED,
    FLOAT,
    DOUBLE,
    BOOLEAN,
    STRING,
    ARRAY,
    RECORD,
    ANY
  };

  using Byte = std::uint8_t;
  using Integer = std::int32_t;
  using Unsigned = std::uint32_t;
  using Float = float;
  using Double = double;
  using Boolean = bool;
  using String = std::string;
  using Array = Array<Value>;
  using Record = Map<String, Value>;
  using Any = std::any;
  using CString = const char *;

private:
  TYPE _type;

  union {
    Byte _byte;
    Integer _integer;
    Unsigned _unsigned;
    Float _float;
    Double _double;
    Boolean _boolean;
  };
  String _string;
  Array _array;
  Record _record;
  Any _any;

protected:
  Value toByteValue() const;
  Value toIntegerValue() const;
  Value toUnsignedValue() const;
  Value toFloatValue() const;
  Value toDoubleValue() const;
  Value toBooleanValue() const;
  Value toStringValue() const;

public:
  Value();
  Value(std::nullptr_t);
  Value(Byte value);
  Value(Integer value);
  Value(Unsigned value);
  Value(Float value);
  Value(Double value);
  Value(Boolean value);
  Value(CString value);
  Value(const String &value);
  Value(const Array &value);
  Value(const Record &value);

  template <typename T>
  Value(T &&value) : _type(Value::TYPE::ANY), _any(value) {}

  Value(const Value &another);

  virtual ~Value();

  Value &operator=(std::nullptr_t);
  Value &operator=(Byte value);
  Value &operator=(Integer value);
  Value &operator=(Unsigned value);
  Value &operator=(Float value);
  Value &operator=(Double value);
  Value &operator=(Boolean value);
  Value &operator=(CString value);
  Value &operator=(const String &value);
  Value &operator=(const Array &value);
  Value &operator=(const Record &value);
  Value &operator=(const Value &another);
  template <typename T> Value &operator=(T &&value) {
    clear();
    _type = TYPE::ANY;
    _any = value;
    return *this;
  }

  void clear();

  inline const TYPE &getType() const { return _type; }

  template <TYPE type> Value to() const {
    switch (type) {
    case TYPE::BYTE:
      return toByteValue();
    case TYPE::INTEGER:
      return toIntegerValue();
    case TYPE::UNSIGNED:
      return toUnsignedValue();
    case TYPE::FLOAT:
      return toFloatValue();
    case TYPE::DOUBLE:
      return toDoubleValue();
    case TYPE::BOOLEAN:
      return toBooleanValue();
    case TYPE::STRING:
      return toStringValue();
    default:
      break;
    }
    return nullptr;
  };

  template <TYPE type> constexpr auto as() const {
    if (type != _type) {
      throw std::bad_cast();
    }
    if constexpr (type == TYPE::BYTE) {
      return _byte;
    } else if constexpr (type == TYPE::INTEGER) {
      return _integer;
    } else if constexpr (type == TYPE::UNSIGNED) {
      return _unsigned;
    } else if constexpr (type == TYPE::FLOAT) {
      return _float;
    } else if constexpr (type == TYPE::DOUBLE) {
      return _double;
    } else if constexpr (type == TYPE::BOOLEAN) {
      return _boolean;
    } else if constexpr (type == TYPE::STRING) {
      return _string;
    } else if constexpr (type == TYPE::ARRAY) {
      return _array;
    } else if constexpr (type == TYPE::RECORD) {
      return _record;
    } else if constexpr (type == TYPE::ANY) {
      return _any;
    } else {
      return nullptr;
    }
  }
  std::string toString() const { return to<TYPE::STRING>().as<TYPE::STRING>(); }
};
inline std::ostream &operator<<(std::ostream &out, const core::Value &value) {
  out << value.toString();
  return out;
}

} // namespace firefly::core
