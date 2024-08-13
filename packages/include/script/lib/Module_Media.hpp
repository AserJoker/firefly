#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Media {
private:
  static FUNC_DEF(query);
  static FUNC_DEF(createBuffer);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script