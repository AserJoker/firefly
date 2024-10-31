#pragma once
#include "Any.hpp"
#include "CompileString.hpp"
#include "Type.hpp"
#include <exception>
#include <initializer_list>
#include <string_view>
#include <type_traits>
#include <utility>

namespace firefly::core {

template <class T, auto v, core::CompileString n, class V = T> struct Type {
  using valueType = T;
  using keyType = decltype(v);
  static constexpr auto value = v;
  static constexpr std::string_view name = n.value;
};

template <typename... TYPES> class BaseValue {
public:
  using KeyType = KeyType<TYPES...>::type;

public:
  static constexpr std::initializer_list<std::pair<KeyType, std::string_view>>
      types = {{TYPES::value, TYPES::name}...};

protected:
  Any _value;
  KeyType _type;
  core::String_t _typename;

public:
  template <class T>
  using Select =
      TypeSelect<std::remove_cv_t<std::remove_reference_t<T>>, TYPES...>::type;

public:
  BaseValue() : _value(nullptr), _type{} {}

  template <typename T>
  BaseValue(T value)
      : _value(value), _type(Select<T>::value), _typename(Select<T>::name) {}

  BaseValue(const BaseValue &another)
      : _value(another._value), _type(another._type),
        _typename(another._typename) {}

  template <typename T> operator T() const { return get<T>(); }

  BaseValue &operator=(const BaseValue &value) {
    _value = value._value;
    _type = value._type;
    _typename = value._typename;
    return *this;
  }

  core::Boolean_t operator==(const BaseValue &another) const {
    try {
      return _value == another._value;
    } catch (std::exception &e) {
      return false;
    }
  }
  core::Boolean_t operator!=(const BaseValue &another) const {
    return !(*this == another);
  }

  template <typename T> inline const T &get() const { return _value.as<T>(); }
  template <typename T> inline T &get() { return _value.as<T>(); }

  inline KeyType getType() const { return _type; }

  inline const core::String_t &getTypeName() const { return _typename; }
};
}; // namespace firefly::core