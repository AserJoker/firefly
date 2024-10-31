//
// Created by w30029682 on 2024/7/4.
//

#include "runtime/CmdLine.hpp"
#include <algorithm>
#include <fmt/format.h>
using namespace firefly;
using namespace firefly::runtime;

core::Map<core::String_t, core::String_t>
CmdLine::parse(const core::Array<core::String_t> &args) {
  core::Map<core::String_t, core::String_t> parsed;
  Param *param = nullptr;
  for (const auto &arg : args) {
    if (arg.starts_with("-")) {
      if (arg.starts_with("--")) {
        auto name = arg.substr(2);
        auto result =
            std::find(_params.begin(), _params.end(), Param{.name = name});
        if (result != _params.end()) {
          if (param) {
            param->setter("");
            parsed[param->name] = "";
          }
          param = &*result;
        }
      } else {
        auto alias = arg.substr(1);
        auto result =
            std::find(_params.begin(), _params.end(), Param{.alias = alias});
        if (result != _params.end()) {
          if (param) {
            param->setter("");
            parsed[param->name] = "";
          }
          param = &*result;
        }
      }
    } else if (param != nullptr) {
      param->setter(arg);
      parsed[param->name] = arg;
      param = nullptr;
    }
  }
  if (param) {
    parsed[param->name] = "";
  }
  _parsed = parsed;
  return parsed;
}

core::String_t CmdLine::help() {
  core::String_t result;
  for (auto &param : _params) {
    core::String_t item;
    if (param.alias.empty()) {
      item = fmt::format("--{}", param.name);
    } else {
      item = fmt::format("--{},-{}", param.name, param.alias);
    }
    if (item.length() < 20) {
      item += core::String_t(20 - item.length(), ' ');
    } else {
      item += ' ';
    }
    item += param.description;
    result += fmt::format("  {}\n", item);
  }
  return result;
}