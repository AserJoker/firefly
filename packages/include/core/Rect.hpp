#pragma once
#include <glm/glm.hpp>
namespace firefly::core {
template <class T = int32_t> struct Point {
  T x;
  T y;
  T z;

  Point() : x(T{}), y(T{}), z(T{}) {}

  Point(T x, T y, T z = T{}) : x(x), y(y), z(z) {}

  Point(const Point<T> &another) : x(another.x), y(another.y), z(another.z) {}

  Point<T> &operator=(const Point<T> &another) {
    x = another.x;
    y = another.y;
    z = another.z;
    return *this;
  }

  operator glm::vec<3, T>() { return {x, y, z}; }

  bool operator==(const Point<T> &another) const {
    if (&another == this) {
      return true;
    }
    return x == another.x && y == another.y && z == another.z;
  }

  Point<T> operator+(const Point &another) const {
    return {x + another.x, y + another.y, z + another.z};
  }

  Point<T> &operator+=(const Point &another) {
    x += another.x;
    y += another.y;
    z += another.z;
    return *this;
  }

  Point<float> operator+(float value) const {
    return {x + value, y + value, z + value};
  }

  Point<T> operator+(int32_t value) const {
    return {x + value, y + value, z + value};
  }

  Point<T> &operator+=(T value) {
    x += value;
    y += value;
    z += value;
    return *this;
  }

  Point<T> operator-() const { return {-x, -y, -z}; }

  Point<T> operator-(const Point &another) const { return *this + (-another); }

  Point<float> operator-(float value) const { return *this + (-value); }

  Point<T> operator-(int32_t value) const { return *this + (-value); }

  Point<T> &operator-=(const Point &another) { return *this += (-another); }

  Point<T> &operator-=(T value) { return *this += (-value); }

  Point<float> operator*(const float &value) const {
    return {x * value, y * value, z * value};
  }

  Point<T> operator*(const int32_t &value) const {
    return {x * value, y * value, z * value};
  }

  Point<T> &operator*=(T value) {
    x *= value;
    y *= value;
    z *= value;
    return *this;
  }
  template <class K> operator Point<K>() { return {(K)x, (K)y, (K)z}; }
};
template <class T = uint32_t> struct Size {
  T width;
  T height;
  T depth;

  Size() : width(T{}), height(T{}), depth(T{}) {}

  Size(T width, T height, T depth = T{})
      : width(width), height(height), depth(depth) {}

  Size(const Size<T> &another)
      : width(another.width), height(another.height), depth(another.depth) {}

  Size<T> &operator=(const Size<T> &another) {
    width = another.width;
    height = another.height;
    depth = another.depth;
    return *this;
  }

  operator glm::vec<3, T>() { return {width, height, depth}; }

  bool operator==(const Size<T> &another) const {
    if (&another == this) {
      return true;
    }
    return width == another.width && height == another.height &&
           depth == another.depth;
  }

  Size<T> operator+(const Size<T> &another) const {
    return {width + another.width, height + another.height,
            depth + another.depth};
  }

  Size<float> operator+(const float &value) const {
    return {width + value, height + value, depth + value};
  }

  Size<T> operator+(const int32_t &value) const {
    return {width + value, height + value, depth + value};
  }

  Size<T> &operator+=(const Size<T> &another) {
    width += another.width;
    height += another.height;
    depth += another.depth;
    return *this;
  }

  Size<T> &operator+=(T value) {
    width += value;
    height += value;
    depth += value;
    return *this;
  }

  Size<T> operator-(const Size<T> &another) const {
    return {width - another.width, height - another.height,
            depth - another.depth};
  }

  Size<T> operator-(int32_t value) const {
    return {width - value, height - value, depth - value};
  }

  Size<float> operator-(float value) const {
    return {width - value, height - value, depth - value};
  }

  Size<T> operator-=(const Size<T> &another) {
    width -= another.width;
    height -= another.height;
    depth -= another.depth;
    return *this;
  }

  Size<T> operator-=(T value) {
    width -= value;
    height -= value;
    depth -= value;
    return *this;
  }

  Size<T> operator*(const Size<T> &another) const {
    return {width * another.width, height * another.height,
            depth * another.depth};
  }

  Size<float> operator*(float value) const {
    return {width * value, height * value, depth * value};
  }

  Size<T> operator*(int32_t value) const {
    return {width * value, height * value, depth * value};
  }

  Size<T> &operator*=(T value) {
    width *= value;
    height *= value;
    depth *= value;
    return *this;
  }

  template <class K> operator Size<K>() {
    return {(K)width, (K)height, (K)depth};
  }
};
template <class T = int32_t, class K = uint32_t>
struct Rect : public Point<T>, Size<K> {
  Rect() : Point<T>(), Size<K>() {}

  Rect(T x, T y, T z, K width, K height, K depth)
      : Point<T>(x, y, z), Size<K>(width, height, depth) {}
  Rect(T x, T y, K width, K height) : Point<T>(x, y), Size<K>(width, height) {}
  Rect(const Point<T> &point, K width, K height, K depth)
      : Point<T>(point), Size<K>(width, height, depth) {}
  Rect(T x, T y, T z, const Size<K> &size) : Point<T>(x, y, z), Size<K>(size) {}
  Rect(const Point<T> &point, const Size<K> &size)
      : Point<T>(point), Size<K>(size) {}

  operator Point<T>() { return {this->x, this->y, this->z}; }
  operator Size<K>() { return {this->width, this->height, this->depth}; }

  bool operator==(const Rect<T, K> &another) {
    if (this == &another) {
      return true;
    }
    return Point<T>(another) == Point<T>(*this) &&
           Size<K>(another) == Size<K>(*this);
  }
};
}; // namespace firefly::core