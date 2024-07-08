//
// Created by w30029682 on 2024/7/4.
//

#pragma once

#include "core/Object.hpp"
#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace firefly::runtime {
class CmdLine : public core::Object {
public:
  template <class T> class Receive {
  private:
    T *_value;

  public:
    explicit Receive(T *value) : _value(value) {}
    Receive &operator=(const std::string &source) {
      std::stringstream ss(source);
      ss >> *_value;
      return *this;
    }
  };

private:
  struct Param {
    std::string name;
    std::string alias;
    std::function<void(const std::string &)> setter;
    std::string description;
    auto operator==(const Param &another) const {
      if (!another.alias.empty()) {
        return alias == another.alias;
      }
      return name == another.name;
    }
    auto operator!=(const Param &another) const {
      if (!another.alias.empty()) {
        return alias != another.alias;
      }
      return name != another.name;
    }
  };
  std::vector<Param> _params;
  std::unordered_map<std::string, std::string> _parsed;

public:
  template <class T>
  void define(std::string name, T &value, std::string alias = "",
              std::string description = "") {
    Param param = {name, alias,
                   [&](const std::string &source) -> void { value = source; },
                   description};
    _params.push_back(param);
  }
  std::unordered_map<std::string, std::string>
  parse(const std::vector<std::string> &args);
  std::string help();
  inline const auto &parsed() { return _parsed; }

public:
  std::string placeholder;
};
} // namespace firefly::runtime
