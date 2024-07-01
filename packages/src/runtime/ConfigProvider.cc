#include "runtime/ConfigProvider.hpp"
#include <yaml-cpp/yaml.h>
using namespace firefly::runtime;
void ConfigProvider::parse(const std::string &path) {
  for (auto &[file, setters] : _schemas) {
    try {
      YAML::Node root = YAML::LoadFile(path + "/" + file);
      for (auto &setter : setters) {
        setter(&root);
      }
    } catch (...) {
      for (auto &setter : setters) {
        setter(nullptr);
      }
    }
  }
}