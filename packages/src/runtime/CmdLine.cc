//
// Created by w30029682 on 2024/7/4.
//

#include <fmt/format.h>
#include "runtime/CmdLine.hpp"

using namespace firefly::runtime;

std::unordered_map<std::string, std::string> CmdLine::parse(const std::vector<std::string> &args) {
    std::unordered_map<std::string, std::string> parsed;
    Param *param = nullptr;
    for (const auto &arg: args) {
        if (arg.starts_with("-")) {
            if (arg.starts_with("--")) {
                auto name = arg.substr(2);
                auto result = std::find(_params.begin(), _params.end(), Param{.name = name});
                if (result != _params.end()) {
                    if (param) {
                        param->setter("");
                        parsed[param->name] = "";
                    }
                    param = &*result;
                }
            } else {
                auto alias = arg.substr(1);
                auto result = std::find(_params.begin(), _params.end(), Param{.alias = alias});
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

std::string CmdLine::help() {
    std::string result;
    for (auto &param: _params) {
        std::string item;
        if (param.alias.empty()) {
            item = fmt::format("--{}", param.name);
        } else {
            item = fmt::format("--{},-{}", param.name, param.alias);
        }
        if (item.length() < 20) {
            item += std::string(20 - item.length(), ' ');
        } else {
            item += ' ';
        }
        item += param.description;
        result += fmt::format("  {}\n", item);
    }
    return result;
}