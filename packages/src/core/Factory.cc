#include "core/Factory.hpp"
#include "core/ObjectBase.hpp"
using namespace firefly;
using namespace firefly::core;
std::unordered_map<std::string, std::function<ObjectBase *()>>
    Factory::_creators;
void Factory::provide(const std::string &name,
                      const std::function<ObjectBase *()> &creator) {
  _creators[name] = creator;
}
ObjectBase *Factory::create(const std::string &name) {
  if (_creators.contains(name)) {
    return _creators.at(name)();
  }
  return nullptr;
}

bool Factory::exist(const std::string &name) {
  return _creators.contains(name);
}