#pragma once
#include "core/Value.hpp"
namespace firefly::core {
class Json {
public:
  static Value parse(const String_t &source);
  static String_t stringify(const Value &object);
};
}; // namespace firefly::core