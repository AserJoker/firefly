#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Video {
private:
  static FUNC_DEF(createSprite2D);
  static FUNC_DEF(setShader);

public:
  static void open(core::AutoPtr<Script> ctx);
};
} // namespace firefly::script