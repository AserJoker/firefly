#pragma once
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
namespace firefly::script {
class Module_Log {
private:
  static FUNC_DEF(debug);
  static FUNC_DEF(info);
  static FUNC_DEF(log);
  static FUNC_DEF(warn);
  static FUNC_DEF(error);
  static FUNC_DEF(panic);

public:
  static void open(core::AutoPtr<Script> ctx);
};
} // namespace firefly::script