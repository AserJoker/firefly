#pragma once
#include <cstdint>
namespace firefly::core {
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
}; // namespace firefly::core