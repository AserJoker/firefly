#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Input {
private:
  static FUNC_DEF(captureMouse);
  static FUNC_DEF(releaseMouse);
  static FUNC_DEF(getMousePosition);
  static FUNC_DEF(getKeyState);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script