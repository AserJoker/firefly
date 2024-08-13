#pragma once

#include "script/Script.hpp"
namespace firefly::script {
class Module_Serialization {
private:
  static FUNC_DEF(JSON_stringify);
  static FUNC_DEF(JSON_parse);

  static FUNC_DEF(YAML_stringify);
  static FUNC_DEF(YAML_parse);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script