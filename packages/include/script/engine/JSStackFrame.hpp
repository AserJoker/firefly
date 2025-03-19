#pragma once
#include "../util/JSPosition.hpp"
struct JSStackFrame {
  JSPosition position;
  std::wstring filename;
};