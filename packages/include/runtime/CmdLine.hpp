//
// Created by w30029682 on 2024/7/4.
//

#pragma once

#include "core/Array.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include <functional>
#include <sstream>

namespace firefly::runtime {
class CmdLine : public core::Object {
public:
  template <class T> class Receive {
  private:
    T *_value;

  public:
    explicit Receive(T *value) : _value(value) {}
    Receive &operator=(const core::String_t &source) {
      std::stringstream ss(source);
      ss >> *_value;
      return *this;
    }
  };

private:
  struct Param {
    core::String_t name;
    core::String_t alias;
    std::function<void(const core::String_t &)> setter;
    core::String_t description;
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
  core::Array<Param> _params;
  core::Map<core::String_t, core::String_t> _parsed;

public:
  template <class T>
  void define(core::String_t name, T &value, core::String_t alias = "",
              core::String_t description = "") {
    Param param = {
        name, alias,
        [&](const core::String_t &source) -> void { value = source; },
        description};
    _params.pushBack(param);
  }
  core::Map<core::String_t, core::String_t>
  parse(const core::Array<core::String_t> &args);
  core::String_t help();
  inline const auto &parsed() { return _parsed; }

public:
  core::String_t placeholder;
};
} // namespace firefly::runtime
