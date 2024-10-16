//
// Created by w30029682 on 2024/7/4.
//

#pragma once

#include "AutoPtr.hpp"
#include "Map.hpp"
#include "Object.hpp"
#include "Singleton.hpp"
#include "core/TemplateCString.hpp"
#include <functional>
#include <string>

namespace firefly::core {
class Provider : public Object {
private:
  core::Map<std::string, std::function<AutoPtr<Object>()>> _constructors;

public:
  template <class T, CompileString name> void provide() {
    _constructors[name.value] = []() -> AutoPtr<Object> {
      return Singleton<T>::instance().template cast<Object>();
    };
  }
  template <class T, CompileString name> void provide(T &&func) {
    _constructors[name.value] = func;
  }

  template <class T, class... Args>
  AutoPtr<T> inject(const std::string &name, Args &&...args) {
    if (_constructors.contains(name)) {
      auto &constructor = _constructors.at(name);
      auto ins = constructor().cast<T>();
      ins->initialize(args...);
      return ins;
    }
    return nullptr;
  }
};
} // namespace firefly::core