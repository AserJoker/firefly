#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Database {
private:
  static FUNC_DEF(queryOne);
  static FUNC_DEF(queryList);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script