#pragma once
#include "JSValue.hpp"
#include <cstddef>
struct JSEvalContext {
  size_t pc;
  std::vector<JSValue *> stack;
  JSValue *self{};
  JSValue *clazz{};
};