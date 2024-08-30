#pragma once
#include "core/Object.hpp"
namespace firefly::core {
template <class T> class Pack : public Object {
private:
  T _data;

public:
  Pack() = default;
  Pack(const T &init) : _data(init) {}
  const T &getData() const { return _data; }
  T &getData() { return _data; }
};
} // namespace firefly::core