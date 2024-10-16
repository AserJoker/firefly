//
// Created by w30029682 on 2024/7/4.
//

#pragma once

#include "AutoPtr.hpp"
#include "Provider.hpp"
#include "exception/InjectorException.hpp"
#include <fmt/format.h>

namespace firefly::core {
template <class T, CompileString name> class Injector : public AutoPtr<T> {
public:
  Injector()
      : AutoPtr<T>(
            Singleton<Provider>::instance()->template inject<T>(name.value)) {
    if (!*this) {
      throw exception::InjectorException(
          fmt::format("'{}' is not initialized", name.value));
    }
  }
};
} // namespace firefly::core