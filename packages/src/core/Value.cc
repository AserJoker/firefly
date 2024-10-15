#include "core/Value.hpp"
#include <sstream>
using namespace firefly;
using namespace firefly::core;
Value::Value() : _type(TYPE::NIL) {}
Value::Value(std::nullptr_t) : Value(){};
Value::Value(Byte value) : _type(TYPE::BYTE), _byte(value){};
Value::Value(Integer value) : _type(TYPE::INTEGER), _integer(value){};
Value::Value(Unsigned value) : _type(TYPE::UNSIGNED), _unsigned(value){};
Value::Value(Float value) : _type(TYPE::FLOAT), _float(value){};
Value::Value(Double value) : _type(TYPE::DOUBLE), _double(value){};
Value::Value(Boolean value) : _type(TYPE::BOOLEAN), _boolean(value){};
Value::Value(CString value) : _type(TYPE::STRING), _string(value){};
Value::Value(const String &value) : _type(TYPE::STRING), _string(value){};
Value::Value(const Array &value) : _type(TYPE::ARRAY), _array(value){};
Value::Value(const Record &value) : _type(TYPE::RECORD), _record(value){};
Value::Value(const Value &another) : _type(another._type) {
  switch (_type) {
  case TYPE::NIL:
    break;
  case TYPE::BYTE:
    _byte = another._byte;
    break;
  case TYPE::INTEGER:
    _integer = another._integer;
    break;
  case TYPE::UNSIGNED:
    _unsigned = another._unsigned;
    break;
  case TYPE::FLOAT:
    _float = another._float;
    break;
  case TYPE::DOUBLE:
    _double = another._double;
    break;
  case TYPE::BOOLEAN:
    _boolean = another._boolean;
    break;
  case TYPE::STRING:
    _string = another._string;
    break;
  case TYPE::ARRAY:
    _array = another._array;
    break;
  case TYPE::RECORD:
    _record = another._record;
    break;
  case TYPE::ANY:
    _any = another._any;
    break;
  default:
    break;
  }
}
Value::~Value() {}

void Value::clear() {
  if (_type == TYPE::STRING) {
    _string.clear();
  } else if (_type == TYPE::ARRAY) {
    _array.clear();
  } else if (_type == TYPE::RECORD) {
    _record.clear();
  } else if (_type == TYPE::ANY) {
    _any.reset();
  }
}

Value &Value::operator=(std::nullptr_t) {
  clear();
  _type = TYPE::NIL;
  return *this;
}
Value &Value::operator=(Byte value) {
  clear();
  _type = TYPE::BYTE;
  _byte = value;
  return *this;
}
Value &Value::operator=(Integer value) {
  clear();
  _type = TYPE::INTEGER;
  _integer = value;
  return *this;
}
Value &Value::operator=(Unsigned value) {
  clear();
  _type = TYPE::UNSIGNED;
  _unsigned = value;
  return *this;
}
Value &Value::operator=(Float value) {
  clear();
  _type = TYPE::FLOAT;
  _float = value;
  return *this;
}
Value &Value::operator=(Double value) {
  clear();
  _type = TYPE::DOUBLE;
  _double = value;
  return *this;
}
Value &Value::operator=(Boolean value) {
  clear();
  _type = TYPE::BOOLEAN;
  _boolean = value;
  return *this;
}
Value &Value::operator=(CString value) {
  clear();
  _type = TYPE::STRING;
  _string = value;
  return *this;
}
Value &Value::operator=(const String &value) {
  clear();
  _type = TYPE::STRING;
  _string = value;
  return *this;
}
Value &Value::operator=(const Array &value) {
  clear();
  _type = TYPE::ARRAY;
  _array = value;
  return *this;
}
Value &Value::operator=(const Record &value) {
  clear();
  _type = TYPE::RECORD;
  _record = value;
  return *this;
}
Value &Value::operator=(const Value &another) {
  clear();
  switch (_type) {
  case TYPE::NIL:
    break;
  case TYPE::BYTE:
    _byte = another._byte;
    break;
  case TYPE::INTEGER:
    _integer = another._integer;
    break;
  case TYPE::UNSIGNED:
    _unsigned = another._unsigned;
    break;
  case TYPE::FLOAT:
    _float = another._float;
    break;
  case TYPE::DOUBLE:
    _double = another._double;
    break;
  case TYPE::BOOLEAN:
    _boolean = another._boolean;
    break;
  case TYPE::STRING:
    _string = another._string;
    break;
  case TYPE::ARRAY:
    _array = another._array;
    break;
  case TYPE::RECORD:
    _record = another._record;
    break;
  case TYPE::ANY:
    _any = another._any;
    break;
  default:
    break;
  }
  return *this;
}
Value Value::toByteValue() const {
  switch (_type) {
  case TYPE::BYTE:
    return _byte;
  case TYPE::INTEGER:
    return (Byte)_integer;
  case TYPE::UNSIGNED:
    return (Byte)_unsigned;
  case TYPE::FLOAT:
    return (Byte)_float;
  case TYPE::DOUBLE:
    return (Byte)_double;
  case TYPE::BOOLEAN:
    return (Byte)_boolean;
  case TYPE::STRING: {
    Byte result;
    std::stringstream ss(_string);
    ss >> result;
    return result;
  }
  default:
    break;
  }
  return nullptr;
}
Value Value::toIntegerValue() const {
  switch (_type) {
  case TYPE::BYTE:
    return (Integer)_byte;
  case TYPE::INTEGER:
    return _integer;
  case TYPE::UNSIGNED:
    return (Integer)_unsigned;
  case TYPE::FLOAT:
    return (Integer)_float;
  case TYPE::DOUBLE:
    return (Integer)_double;
  case TYPE::BOOLEAN:
    return (Integer)_boolean;
  case TYPE::STRING: {
    Integer result;
    std::stringstream ss(_string);
    ss >> result;
    return result;
  }
  default:
    break;
  }
  return nullptr;
}
Value Value::toUnsignedValue() const {
  switch (_type) {
  case TYPE::BYTE:
    return (Unsigned)_byte;
  case TYPE::INTEGER:
    return (Unsigned)_integer;
  case TYPE::UNSIGNED:
    return _unsigned;
  case TYPE::FLOAT:
    return (Unsigned)_float;
  case TYPE::DOUBLE:
    return (Unsigned)_double;
  case TYPE::BOOLEAN:
    return (Unsigned)_boolean;
  case TYPE::STRING: {
    Unsigned result;
    std::stringstream ss(_string);
    ss >> result;
    return result;
  }
  default:
    break;
  }
  return nullptr;
}
Value Value::toFloatValue() const {
  switch (_type) {
  case TYPE::BYTE:
    return (Float)_byte;
  case TYPE::INTEGER:
    return (Float)_integer;
  case TYPE::UNSIGNED:
    return (Float)_unsigned;
  case TYPE::FLOAT:
    return _float;
  case TYPE::DOUBLE:
    return (Float)_double;
  case TYPE::BOOLEAN:
    return (Float)_boolean;
  case TYPE::STRING: {
    Float result;
    std::stringstream ss(_string);
    ss >> result;
    return result;
  }
  default:
    break;
  }
  return nullptr;
}
Value Value::toDoubleValue() const {
  switch (_type) {
  case TYPE::BYTE:
    return (Double)_byte;
  case TYPE::INTEGER:
    return (Double)_integer;
  case TYPE::UNSIGNED:
    return (Double)_unsigned;
  case TYPE::FLOAT:
    return (Double)_float;
  case TYPE::DOUBLE:
    return _double;
  case TYPE::BOOLEAN:
    return (Double)_boolean;
  case TYPE::STRING: {
    Double result;
    std::stringstream ss(_string);
    ss >> result;
    return result;
  }
  default:
    break;
  }
  return nullptr;
}
Value Value::toBooleanValue() const {
  switch (_type) {
  case TYPE::BYTE:
    return _byte == 1;
  case TYPE::INTEGER:
    return _integer == 1;
  case TYPE::UNSIGNED:
    return _unsigned == 1;
  case TYPE::FLOAT:
    return _float == 1;
  case TYPE::DOUBLE:
    return _double == 1;
  case TYPE::BOOLEAN:
    return _boolean;
  case TYPE::STRING:
    return _string == "True" || _string == "true" || _string == "TRUE";
  default:
    break;
  }
  return false;
}
Value Value::toStringValue() const {
  std::stringstream ss;
  switch (_type) {
  case TYPE::NIL:
    return "nil";
  case TYPE::BYTE: {
    ss << _byte;
    return ss.str();
  }
  case TYPE::INTEGER: {
    ss << _integer;
    return ss.str();
  }
  case TYPE::UNSIGNED: {
    ss << _unsigned;
    return ss.str();
  }
  case TYPE::FLOAT: {
    ss << _float;
    return ss.str();
  }
  case TYPE::DOUBLE: {
    ss << _double;
    return ss.str();
  }
  case TYPE::BOOLEAN: {
    ss << (_boolean == true ? "true" : "false");
    return ss.str();
  }
  case TYPE::STRING:
    return _string;
  case TYPE::ARRAY:
    return _array.toString();
  case TYPE::RECORD:
    return _record.toString();
  default:
    break;
  }
  return "[Invalid Value]";
}