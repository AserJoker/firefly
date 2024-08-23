#include "core/Object.hpp"
using namespace firefly;
using namespace firefly::core;
std::unordered_map<std::string, Object *> Object::_objects;
Object *Object::select(const std::string &id) {
  if (_objects.contains(id)) {
    return _objects.at(id);
  } else {
    return nullptr;
  }
}