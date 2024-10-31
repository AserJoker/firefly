#pragma once
#include "Type.hpp"
namespace firefly::core {
template <class T = core::Unsigned_t> struct Size {
  T width;
  T height;

  constexpr Size() : width(T{}), height(T{}) {}

  constexpr Size(T width, T height) : width(width), height(height) {}

  Size(const Size<T> &another) : width(another.width), height(another.height) {}

  Size<T> &operator=(const Size<T> &another) {
    width = another.width;
    height = another.height;
    return *this;
  }

  core::Boolean_t operator==(const Size<T> &another) const {
    if (&another == this) {
      return true;
    }
    return width == another.width && height == another.height;
  }

  Size<T> operator+(const Size<T> &another) const {
    return {width + another.width, height + another.height};
  }

  Size<core::Float_t> operator+(const core::Float_t &value) const {
    return {width + value, height + value};
  }

  Size<T> operator+(const core::Integer_t &value) const {
    return {width + value, height + value};
  }

  Size<T> &operator+=(const Size<T> &another) {
    width += another.width;
    height += another.height;
    return *this;
  }

  Size<T> &operator+=(T value) {
    width += value;
    height += value;
    return *this;
  }

  Size<T> operator-(const Size<T> &another) const {
    return {width - another.width, height - another.height};
  }

  Size<T> operator-(core::Integer_t value) const {
    return {width - value, height - value};
  }

  Size<core::Float_t> operator-(core::Float_t value) const {
    return {width - value, height - value};
  }

  Size<T> &operator-=(const Size<T> &another) {
    width -= another.width;
    height -= another.height;
    return *this;
  }

  Size<T> &operator-=(T value) {
    width -= value;
    height -= value;
    return *this;
  }

  Size<T> operator*(const Size<T> &another) const {
    return {width * another.width, height * another.height};
  }

  Size<core::Float_t> operator*(core::Float_t value) const {
    return {width * value, height * value};
  }

  Size<T> operator*(core::Integer_t value) const {
    return {width * value, height * value};
  }

  Size<T> &operator*=(T value) {
    width *= value;
    height *= value;
    return *this;
  }

  template <class K> operator Size<K>() { return {(K)width, (K)height}; }
};
}; // namespace firefly::core