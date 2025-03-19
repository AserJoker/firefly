#pragma once
#include "ObjectBase.hpp"
#include <functional>
#include <string>
#include <unordered_map>
namespace firefly::core {
class Factory {
private:
  static std::unordered_map<std::string, std::function<ObjectBase *()>>
      _creators;

public:
  static void provide(const std::string &name,
                      const std::function<ObjectBase *()> &creator);
                      
  static ObjectBase *create(const std::string &name);

  static bool exist(const std::string &name);
};
}; // namespace firefly::core