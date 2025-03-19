
#pragma once
#include "JSPosition.hpp"
#include <string>
struct JSLocation {
  std::wstring filename;
  JSPosition start;
  JSPosition end;
  bool is(const std::wstring &source, const std::wstring &pair) const {
    if (end.offset - start.offset != pair.length()) {
      return false;
    }
    for (size_t i = 0; i < pair.size(); i++) {
      if (source[i + start.offset] != pair[i]) {
        return false;
      }
    }
    return true;
  }
  std::wstring get(const std::wstring &source) const {
    std::wstring result;
    for (auto i = start.offset; i < end.offset; i++) {
      result += source[i];
    }
    return result;
  }
};