#pragma once
#include "Type.hpp"
namespace firefly::core {
template <class T = core::Integer_t> struct Point {
  T x;
  T y;

  constexpr Point() : x(T{}), y(T{}) {}

  constexpr Point(T x, T y, T z = T{}) : x(x), y(y) {}

  Point(const Point<T> &another) : x(another.x), y(another.y) {}

  Point<T> &operator=(const Point<T> &another) {
    x = another.x;
    y = another.y;
    return *this;
  }

  core::Boolean_t operator==(const Point<T> &another) const {
    if (&another == this) {
      return true;
    }
    return x == another.x && y == another.y;
  }

  Point<T> operator+(const Point &another) const {
    return {x + another.x, y + another.y};
  }

  Point<T> &operator+=(const Point &another) {
    x += another.x;
    y += another.y;
    return *this;
  }

  Point<core::Float_t> operator+(core::Float_t value) const {
    return {x + value, y + value};
  }

  Point<T> operator+(core::Integer_t value) const {
    return {x + value, y + value};
  }

  Point<T> &operator+=(T value) {
    x += value;
    y += value;
    return *this;
  }

  Point<T> operator-() const { return {-x, -y}; }

  Point<T> operator-(const Point &another) const { return *this + (-another); }

  Point<core::Float_t> operator-(core::Float_t value) const {
    return *this + (-value);
  }

  Point<T> operator-(core::Integer_t value) const { return *this + (-value); }

  Point<T> &operator-=(const Point &another) { return *this += (-another); }

  Point<T> &operator-=(T value) { return *this += (-value); }

  Point<core::Float_t> operator*(const core::Float_t &value) const {
    return {x * value, y * value};
  }

  Point<T> operator*(const core::Integer_t &value) const {
    return {x * value, y * value};
  }

  Point<T> &operator*=(T value) {
    x *= value;
    y *= value;
    return *this;
  }
  template <class K> operator Point<K>() { return {(K)x, (K)y}; }
};
}; // namespace firefly::core