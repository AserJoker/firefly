//
// Created by w30029682 on 2024/7/4.
//

#pragma once

#include"AutoPtr.hpp"
#include "Provider.hpp"
#include <fmt/format.h>

namespace firefly::core {
    template<class T, template_c_string name>
    class Injector : public AutoPtr<T> {
    public:
        Injector() : AutoPtr<T>(Singleton<Provider>::instance()->template inject<T>(name.value)) {
            if (!*this) {
                throw std::runtime_error(fmt::format("Uninitialized injector: '{}'", name.value));
            }
        }
    };
}