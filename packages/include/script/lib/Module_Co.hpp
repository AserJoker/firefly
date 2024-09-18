#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Co {
private:
  static FUNC_DEF(start);
  static FUNC_DEF(yield);
  static FUNC_DEF(createPromise);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script