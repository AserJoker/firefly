#pragma once
#include "core/Value.hpp"
namespace firefly::core {
template <class T = Float_t> struct Color {
  T r, g, b, a;
  constexpr Color() : r(T{}), g(T{}), b(T{}), a(T{}) {}
  constexpr Color(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}
  constexpr Color(const Color<T> &another)
      : r(another.r), g(another.g), b(another.b), a(another.a) {}

  Color<T> &operator=(const Color<T> &another) {
    r = another.r;
    g = another.g;
    b = another.b;
    a = another.a;
    return *this;
  }

  bool operator==(const Color<T> &another) {
    return r == another.r && g == another.g && b == another.b && a == another.a;
  }
  bool operator!=(const Color<T> &another) { return !(*this == another); }
};
}; // namespace firefly::core