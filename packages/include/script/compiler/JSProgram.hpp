#pragma once
#include "../engine/JSStackFrame.hpp"
#include "JSParser.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

struct JSProgram {
  std::wstring filename;
  std::vector<std::wstring> constants;
  std::vector<uint16_t> codes;
  std::unordered_map<size_t, JSStackFrame> stacks;
  JSErrorNode *error{};
  JSAllocator *allocator{};
  JSProgram() {}
  virtual ~JSProgram() {
    if (error) {
      error = nullptr;
    }
  }
  std::wstring toString();
};