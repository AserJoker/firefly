#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace firefly::core {
template <class T = core::Integer_t, class K = core::Unsigned_t> struct Rect {
  union {
    struct {
      T x, y;
    };
    core::Point<T> point;
  };
  union {
    struct {
      K width, height;
    };
    core::Size<K> size;
  };

  constexpr Rect() : x(T{}), y(T{}), width(K{}), height(K{}) {}

  constexpr Rect(T x, T y, K width, K height)
      : x(x), y(y), width(width), height(height) {}

  Rect(const Point<T> &point, K width, K height)
      : Rect(point.x, point.y, width, height) {}

  Rect(T x, T y, const Size<K> &size) : Rect(x, y, size.width, size.height) {}

  Rect(const Point<T> &point, const Size<K> &size)
      : Rect(point.x, point.y, size.width, size.height) {}

  Rect(const Rect<T, K> &another) : point(another.point), size(another.size) {}

  Rect<T, K> &operator=(const Rect<T, K> &another) {
    point = another.point;
    size = another.size;
    return *this;
  }

  core::Boolean_t operator==(const Rect<T, K> &another) const {
    if (this == &another) {
      return true;
    }
    return point == another.point && size == another.size;
  }

  core::Boolean_t operator!=(const Rect<T, K> &another) const {
    return !(*this == another);
  }
};
}; // namespace firefly::core