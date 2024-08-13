#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Runtime {
private:
  static FUNC_DEF(setWindowTitle);
  static FUNC_DEF(getWindowTitle);
  static FUNC_DEF(setWindowSize);
  static FUNC_DEF(getWindowSize);

  static FUNC_DEF(saveConfig);
  static FUNC_DEF(loadConfig);
  static FUNC_DEF(save);
  static FUNC_DEF(load);
  static FUNC_DEF(getSaveInfo);

  static FUNC_DEF(createProperties);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script