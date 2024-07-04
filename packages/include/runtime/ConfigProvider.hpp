#pragma once
#include "core/Object.hpp"
#include <exception>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <regex>
namespace firefly::runtime {
class ConfigProvider : public core::Object {
private:
  std::unordered_map<std::string,
                     std::vector<std::function<void(YAML::Node *)>>>
      _schemas;

public:
  void parse(const std::string &path);
  template <class T, class D>
  void define(const std::string &name, const std::string &key, T *value,
              const D &defaultValue) {
    auto &schemas = _schemas[name];
    std::vector<std::string> parts;
    std::string part;
    for (const auto &c : key) {
      if (c == '.') {
        if (!part.empty()) {
          parts.push_back(part);
          part = "";
        }
      } else {
        part += c;
      }
    }
    if (!part.empty()) {
      parts.push_back(part);
    }
    schemas.push_back([=](YAML::Node *root) -> void {
      if (!root) {
        *value = defaultValue;
        return;
      }
      YAML::Node tmp = *root;
      for (auto &part : parts) {
        tmp = tmp[part];
      }
      *value = tmp.as<T>();
    });
  }
  template <class T>
  void define(const std::string &name, const std::string &key, T *value) {
    auto &schemas = _schemas[name];
    std::vector<std::string> parts;
    std::string part;
    for (const auto &c : key) {
      if (c == '.') {
        if (!part.empty()) {
          parts.push_back(part);
          part = "";
        }
      } else {
        part += c;
      }
    }
    if (!part.empty()) {
      parts.push_back(part);
    }
    schemas.push_back([=](YAML::Node *root) -> void {
      if (!root) {
        return;
      }
      YAML::Node tmp = *root;
      for (auto &part : parts) {
        tmp = tmp[part];
      }
      *value = tmp.as<T>();
    });
  }
};
} // namespace firefly::runtime