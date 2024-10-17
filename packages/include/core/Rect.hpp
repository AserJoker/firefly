#pragma once
#include <glm/glm.hpp>
namespace firefly::core {
template <class T = int32_t> struct Point {
  T x;
  T y;

  Point() : x(T{}), y(T{}) {}

  Point(T x, T y, T z = T{}) : x(x), y(y) {}

  Point(const Point<T> &another) : x(another.x), y(another.y) {}

  Point<T> &operator=(const Point<T> &another) {
    x = another.x;
    y = another.y;
    return *this;
  }

  operator glm::vec<2, T>() { return {x, y}; }

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
template <class T = uint32_t> struct Size {
  T width;
  T height;

  Size() : width(T{}), height(T{}) {}

  Size(T width, T height) : width(width), height(height) {}

  Size(const Size<T> &another) : width(another.width), height(another.height) {}

  Size<T> &operator=(const Size<T> &another) {
    width = another.width;
    height = another.height;
    return *this;
  }

  operator glm::vec<2, T>() { return {width, height}; }

  bool operator==(const Size<T> &another) const {
    if (&another == this) {
      return true;
    }
    return width == another.width && height == another.height;
  }

  Size<T> operator+(const Size<T> &another) const {
    return {width + another.width, height + another.height};
  }

  Size<float> operator+(const float &value) const {
    return {width + value, height + value};
  }

  Size<T> operator+(const int32_t &value) const {
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

  Size<T> operator-(int32_t value) const {
    return {width - value, height - value};
  }

  Size<float> operator-(float value) const {
    return {width - value, height - value};
  }

  Size<T> operator-=(const Size<T> &another) {
    width -= another.width;
    height -= another.height;
    return *this;
  }

  Size<T> operator-=(T value) {
    width -= value;
    height -= value;
    return *this;
  }

  Size<T> operator*(const Size<T> &another) const {
    return {width * another.width, height * another.height};
  }

  Size<float> operator*(float value) const {
    return {width * value, height * value};
  }

  Size<T> operator*(int32_t value) const {
    return {width * value, height * value};
  }

  Size<T> &operator*=(T value) {
    width *= value;
    height *= value;
    return *this;
  }

  template <class K> operator Size<K>() { return {(K)width, (K)height}; }
};
template <class T = int32_t, class K = uint32_t> struct Rect {
  T x, y;
  K width, height;
  Rect() : x(T{}), y(T{}), width(K{}), height(K{}) {}
  Rect(T x, T y, K width, K height)
      : x(x), y(y), width(width), height(height) {}
  Rect(const Point<T> &point, K width, K height)
      : Rect(point.x, point.y, width, height) {}
  Rect(T x, T y, const Size<K> &size) : Rect(x, y, size.width, size.height) {}
  Rect(const Point<T> &point, const Size<K> &size)
      : Rect(point.x, point.y, size.width, size.height) {}

  operator Point<T>() { return {this->x, this->y}; }
  operator Size<K>() { return {this->width, this->height}; }

  bool operator==(const Rect<T, K> &another) {
    if (this == &another) {
      return true;
    }
    return x == another.x && y == another.y && width == another.width &&
           height == another.height;
  }
};
}; // namespace firefly::core