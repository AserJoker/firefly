#pragma once

#include <cstddef>
#include <string>
struct JSPosition {
  std::wstring funcname;
  size_t line;
  size_t column;
  size_t offset;
};