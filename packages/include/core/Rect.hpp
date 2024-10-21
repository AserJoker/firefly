#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace firefly::core {
template <class T = int32_t, class K = uint32_t> struct Rect {
  T x, y;
  K width, height;

  constexpr Rect() : x(T{}), y(T{}), width(K{}), height(K{}) {}

  constexpr Rect(T x, T y, K width, K height)
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