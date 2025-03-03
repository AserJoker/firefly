#pragma once

// JSON

#include <any>
#include <map>
#include <string>
#include <vector>
enum class JSON_NODE_TYPE { OBJECT, ARRAY, STRING, NUMBER, NIL, BOOLEAN };

class JSON {
private:
  JSON_NODE_TYPE _type;
  std::any _value;
  friend class JSONCheckedNumber;

private:
  using JSONNumberValue = double;
  using JSONStringValue = std::wstring;
  using JSONBooleanValue = bool;
  using JSONNullValue = std::nullptr_t;
  using JSONArrayValue = std::vector<JSON>;
  using JSONObjectValue = std::map<std::wstring, JSON>;

public:
  JSON() : _type(JSON_NODE_TYPE::NIL), _value(nullptr) {}

  JSON(const JSON &another) {
    switch (another._type) {
    case JSON_NODE_TYPE::OBJECT:
      _value = std::any_cast<const JSONObjectValue &>(another._value);
      break;
    case JSON_NODE_TYPE::ARRAY:
      _value = std::any_cast<const JSONArrayValue &>(another._value);
      break;
    case JSON_NODE_TYPE::STRING:
      _value = std::any_cast<const JSONStringValue &>(another._value);
      break;
    case JSON_NODE_TYPE::NUMBER:
      _value = std::any_cast<const JSONNumberValue &>(another._value);
      break;
    case JSON_NODE_TYPE::NIL:
      _value = nullptr;
      break;
    case JSON_NODE_TYPE::BOOLEAN:
      _value = std::any_cast<JSONBooleanValue>(another._value);
      break;
    }
    _type = another._type;
  }

  JSON &setNull() {
    _type = JSON_NODE_TYPE::NIL;
    _value = nullptr;
    return *this;
  }

  JSON &setNumber(double value) {
    _type = JSON_NODE_TYPE::NUMBER;
    _value = value;
    return *this;
  }

  JSON &setBoolean(bool value) {
    _type = JSON_NODE_TYPE::BOOLEAN;
    _value = value;
    return *this;
  }

  JSON &setString(const std::wstring &value) {
    _type = JSON_NODE_TYPE::STRING;
    std::wstring str;
    _value = std::wstring{value};
    return *this;
  }

  JSON &setObject() {
    _type = JSON_NODE_TYPE::OBJECT;
    _value = JSONObjectValue{};
    return *this;
  }

  JSON &setArray() {
    _type = JSON_NODE_TYPE::ARRAY;
    _value = JSONArrayValue{};
    return *this;
  }

  JSONNumberValue &getNumber() {
    return std::any_cast<JSONNumberValue &>(_value);
  }

  JSONStringValue &getString() {
    return std::any_cast<JSONStringValue &>(_value);
  }

  JSONBooleanValue &getBoolean() {
    return std::any_cast<JSONBooleanValue &>(_value);
  }

  const JSONNumberValue &getNumber() const {
    return std::any_cast<const JSONNumberValue &>(_value);
  }

  const JSONStringValue &getString() const {
    return std::any_cast<const JSONStringValue &>(_value);
  }

  const JSONBooleanValue &getBoolean() const {
    return std::any_cast<const JSONBooleanValue &>(_value);
  }

  JSON &setIndex(size_t index, const JSON &item) {
    auto &items = std::any_cast<JSONArrayValue &>(_value);
    while (items.size() <= index) {
      items.push_back(JSON{});
    }
    items[index] = item;
    return *this;
  }

  JSON &getIndex(size_t index) {
    static JSON nil;
    auto &items = std::any_cast<JSONArrayValue &>(_value);
    if (index < items.size()) {
      return items[index];
    }
    return nil;
  }
  size_t getLength() const {
    auto &items = std::any_cast<const JSONArrayValue &>(_value);
    return items.size();
  }
  const JSON &getIndex(size_t index) const {
    static JSON nil;
    auto &items = std::any_cast<const JSONArrayValue &>(_value);
    if (index < items.size()) {
      return items[index];
    }
    return nil;
  }
  JSON &setField(const std::wstring &name, const JSON &field) {
    auto &properties = std::any_cast<JSONObjectValue &>(_value);
    properties[name] = field;
    return *this;
  }
  JSON &getField(const std::wstring &name) {
    static JSON nil;
    auto &properties = std::any_cast<JSONObjectValue &>(_value);
    if (properties.contains(name)) {
      return properties.at(name);
    }
    return nil;
  }
  const JSON &getField(const std::wstring &name) const {
    static JSON nil;
    auto &properties = std::any_cast<const JSONObjectValue &>(_value);
    if (properties.contains(name)) {
      return properties.at(name);
    }
    return nil;
  }
  std::vector<std::wstring> keys() const {
    auto &properties = std::any_cast<const JSONObjectValue &>(_value);
    std::vector<std::wstring> keys;
    for (auto &[k, _] : properties) {
      keys.push_back(k);
    }
    return keys;
  }

  const JSON_NODE_TYPE &getType() const { return _type; }

public:
  static std::wstring stringify(const JSON &node) {
    switch (node._type) {
    case JSON_NODE_TYPE::OBJECT: {
      std::wstring str = L"{";
      auto keys = node.keys();
      for (size_t i = 0; i < keys.size(); i++) {
        if (i != 0) {
          str += L",";
        }
        str += L"\"" + keys[i] + L"\":";
        str += stringify(node.getField(keys[i]));
      }
      str += L"}";
      return str;
    }
    case JSON_NODE_TYPE::ARRAY: {
      std::wstring str = L"[";
      for (size_t i = 0; i < node.getLength(); i++) {
        if (i != 0) {
          str += L",";
        }
        str += stringify(node.getIndex(i));
      }
      str += L"]";
      return str;
    }
    case JSON_NODE_TYPE::STRING: {
      std::wstring str = L"\"";
      for (auto &ch : node.getString()) {
        if (ch == '\n') {
          str += L"\\n";
        } else if (ch == '\r') {
          str += L"\\r";
        } else if (ch == '\"') {
          str += L"\\\"";
        } else {
          str += ch;
        }
      }
      str += L"\"";
      return str;
    }
    case JSON_NODE_TYPE::NUMBER: {
      return std::to_wstring(node.getNumber());
    }
    case JSON_NODE_TYPE::NIL:
      return L"null";
    case JSON_NODE_TYPE::BOOLEAN:
      if (node.getBoolean()) {
        return L"true";
      } else {
        return L"false";
      }
    }
    return L"";
  }
};
