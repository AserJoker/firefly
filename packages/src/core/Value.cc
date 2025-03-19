#include "core/Value.hpp"
using namespace firefly;
using namespace firefly::core;
Value::Value() {
  setNil();
  _bind = false;
}

Value::Value(const std::nullptr_t &) : Value() {}

Value::Value(const std::string &value) : Value() { setString(value); }

Value::Value(const char *value) : Value() { setString(value); }

Value::Value(double value) : Value() { setNumber(value); }

Value::Value(bool value) : Value() { setBoolean(value); }

Value::Value(const Array &value) : Value() {
  setArray();
  for (auto &item : value) {
    getArray().push_back(item);
  }
}

Value::Value(const Object &value) : Value() {
  setObject();
  for (auto &[k, v] : value) {
    getObject()[k] = v;
  }
}

Value::~Value() {}

Value::Value(const Value &another) {
  _bind = another._bind;
  if (_bind) {
    _type = another._type;
    _store = another._store;
  } else {
    *this = another;
  }
}

Value::Value(Value &&another) {
  _type = another._type;
  _store = another._store;
  _bind = another._bind;
  another.setNil();
}

const Value::TYPE &Value::getType() const { return _type; }

bool Value::isBind() const { return _bind; };

void Value::setNil() {
  _type = TYPE::NIL;
  _store = nullptr;
  _bind = false;
}

bool Value::isNil() const { return _type == TYPE::NIL; }

void Value::setArray() {
  if (!_bind) {
    _type = TYPE::ARRAY;
    _store = Array{};
  } else if (_type == TYPE::ARRAY) {
    getArray().clear();
  }
}

bool Value::isArray() const { return _type == TYPE::ARRAY; }

void Value::setObject() {
  if (!_bind) {
    _type = TYPE::OBJECT;
    _store = Object{};
  } else if (_type == TYPE::OBJECT) {
    getObject().clear();
  }
}
bool Value::isObject() const { return _type == TYPE::OBJECT; }

void Value::setString(const std::string &value) {
  if (_bind) {
    if (_type == TYPE::STRING) {
      *std::any_cast<std::string *>(_store) = value;
    }
  } else {
    _type = TYPE::STRING;
    _store = std::string{value};
  }
}

const std::string &Value::checkString() const {
  static std::string def = "";
  if (_type == TYPE::STRING) {
    return getString();
  }
  return def;
}

const std::string &Value::getString() const {
  if (_bind) {
    return *std::any_cast<std::string *>(_store);
  } else {
    return std::any_cast<const std::string &>(_store);
  }
}

void Value::setNumber(double value) {
  if (_bind) {
    if (_type == TYPE::NUMBER) {
      _setNumber(value);
    }
  } else {
    _type = TYPE::NUMBER;
    _store = value;
  }
}

double Value::getNumber() const {
  if (_bind) {
    return _getNumber();
  } else {
    return std::any_cast<double>(_store);
  }
}

double Value::checkNumber() const {
  static double def = .0f;
  if (_type == TYPE::NUMBER) {
    return getNumber();
  }
  return def;
}

void Value::setBoolean(bool value) {
  if (_bind) {
    if (_type == TYPE::BOOLEAN) {
      *std::any_cast<bool *>(_store) = value;
    }
  } else {
    _type = TYPE::BOOLEAN;
    _store = value;
  }
}

bool Value::getBoolean() const {
  if (_bind) {
    return *std::any_cast<bool *>(_store);
  } else {
    return std::any_cast<bool>(_store);
  }
}

bool Value::checkBoolean() const {
  static bool def = false;
  if (_type == TYPE::BOOLEAN) {
    return getBoolean();
  }
  return def;
}

const Value::Array &Value::getArray() const {
  if (_bind) {
    return *std::any_cast<Array *>(_store);
  } else {
    return std::any_cast<const Array &>(_store);
  }
}

Value::Array &Value::getArray() {
  if (_bind) {
    return *std::any_cast<Array *>(_store);
  } else {
    return std::any_cast<Array &>(_store);
  }
}

const Value::Array &Value::checkArray() const {
  static Value::Array def = {};
  if (_type == TYPE::ARRAY) {
    return getArray();
  }
  return def;
}

const Value::Object &Value::getObject() const {
  if (_bind) {
    return *std::any_cast<Object *>(_store);
  } else {
    return std::any_cast<const Object &>(_store);
  }
}

Value::Object &Value::getObject() {
  if (_bind) {
    return *std::any_cast<Object *>(_store);
  } else {
    return std::any_cast<Object &>(_store);
  }
}

const Value::Object &Value::checkObject() const {
  static Value::Object def = {};
  if (_type == TYPE::OBJECT) {
    return getObject();
  }
  return def;
}

Value &Value::operator=(const Value &another) {
  if (this == &another) {
    return *this;
  }
  if (another._bind) {
    if (!_bind) {
      _type = another._type;
      _store = another._store;
      _bind = another._bind;
      return *this;
    } else {
      if (_type != another._type) {
        return *this;
      }
    }
  }
  switch (another._type) {
  case TYPE::NIL:
    setNil();
    break;
  case TYPE::NUMBER:
    setNumber(another.getNumber());
    break;
  case TYPE::STRING:
    setString(another.getString());
    break;
  case TYPE::BOOLEAN:
    setBoolean(another.getBoolean());
    break;
  case TYPE::ARRAY:
    setArray();
    for (auto &item : another.getArray()) {
      getArray().push_back(item);
    }
    break;
  case TYPE::OBJECT:
    setObject();
    for (auto &[key, value] : another.getObject()) {
      getObject()[key] = value;
    }
    break;
  }
  return *this;
}

bool Value::operator==(const Value &another) const {
  if (_type != another._type) {
    return false;
  }
  switch (_type) {
  case TYPE::NIL:
    return true;
  case TYPE::NUMBER:
    return getNumber() == another.getNumber();
  case TYPE::STRING:
    return getString() == another.getString();
  case TYPE::BOOLEAN:
    return getBoolean() == another.getBoolean();
  case TYPE::ARRAY:
    if (getArray().size() != another.getArray().size()) {
      return false;
    }
    for (size_t index = 0; index < getArray().size(); index++) {
      if (getArray()[index] != another.getArray().at(index)) {
        return false;
      }
    }
    break;
  case TYPE::OBJECT:
    if (getObject().size() != another.getObject().size()) {
      return false;
    }
    for (auto &[key, value] : getObject()) {
      if (!another.getObject().contains(key)) {
        return false;
      }
      if (another.getObject().at(key) != value) {
        return false;
      }
    }
    break;
  }
  return true;
};

Value Value::bindString(std::string *src) {
  Value val;
  val._bind = true;
  val._store = src;
  val._type = TYPE::STRING;
  return val;
}

Value Value::bindBoolean(bool *src) {
  Value val;
  val._bind = true;
  val._store = src;
  val._type = TYPE::BOOLEAN;
  return val;
}

Value Value::bindArray(Value::Array *src) {
  Value val;
  val._bind = true;
  val._store = src;
  val._type = TYPE::ARRAY;
  return val;
}

Value Value::bindObject(Value::Object *src) {
  Value val;
  val._bind = true;
  val._store = src;
  val._type = TYPE::OBJECT;
  return val;
}