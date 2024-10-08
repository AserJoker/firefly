#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Video {
private:
  static FUNC_DEF(createSprite2D);
  static FUNC_DEF(createScene);
  static FUNC_DEF(createRenderTarget);
  static FUNC_DEF(createAnimation);
  static FUNC_DEF(setShader);
  static FUNC_DEF(setScene);
  static FUNC_DEF(getScene);

public:
  static void open(core::AutoPtr<Script> ctx);
};
} // namespace firefly::script