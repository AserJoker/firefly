//
// Created by w30029682 on 2024/7/4.
//

#pragma once

#include "Object.hpp"
#include "AutoPtr.hpp"
#include "Singleton.hpp"
#include <string>
#include <unordered_map>
#include <functional>

template<size_t N>
struct template_c_string {
    constexpr template_c_string(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N]{};
};
namespace firefly::core {
    class Provider : public Object {
    private:
        std::unordered_map<std::string, std::function<AutoPtr<Object>()>> _constructors;

    public:
        template<class T, template_c_string name>
        void provide() {
            _constructors[name.value] = []() -> AutoPtr<Object> {
                return Singleton<T>::instance().template cast<Object>();
            };
        }

        template<class T>
        AutoPtr <T> inject(const std::string &name) {
            if (_constructors.contains(name)) {
                auto &constructor = _constructors.at(name);
                return constructor().cast<T>();
            }
            return nullptr;
        }
    };
}