#pragma once
#include <cstdint>
namespace firefly::core {
template <class T = int32_t> struct Point {
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

  bool operator==(const Point<T> &another) const {
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

  Point<float> operator+(float value) const { return {x + value, y + value}; }

  Point<T> operator+(int32_t value) const { return {x + value, y + value}; }

  Point<T> &operator+=(T value) {
    x += value;
    y += value;
    return *this;
  }

  Point<T> operator-() const { return {-x, -y}; }

  Point<T> operator-(const Point &another) const { return *this + (-another); }

  Point<float> operator-(float value) const { return *this + (-value); }

  Point<T> operator-(int32_t value) const { return *this + (-value); }

  Point<T> &operator-=(const Point &another) { return *this += (-another); }

  Point<T> &operator-=(T value) { return *this += (-value); }

  Point<float> operator*(const float &value) const {
    return {x * value, y * value};
  }

  Point<T> operator*(const int32_t &value) const {
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