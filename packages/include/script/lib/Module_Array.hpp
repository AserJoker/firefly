#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Array {
private:
  static FUNC_DEF(createArray);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script