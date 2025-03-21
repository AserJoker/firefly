#pragma once
#include "JSValue.hpp"
#include "script/engine/JSScope.hpp"
#include <cstddef>
#include <string>
struct JSTryFrame {
  JSScope *scope;
  size_t onfinish;
  size_t onerror;
};
struct JSLabelFrame {
  enum class TYPE { BREAK, CONTINUE } type;
  std::wstring label;
  size_t address;
  JSScope *scope{};
  JSTryFrame *tryFrame{};
};
struct JSEvalContext {
  size_t pc;
  std::vector<JSValue *> stack;
  std::vector<size_t> frames;
  std::vector<JSLabelFrame> labels;
  JSValue *self{};
  std::vector<size_t> defer;
  JSValue *result;
  std::vector<JSTryFrame> tryFrames;
};