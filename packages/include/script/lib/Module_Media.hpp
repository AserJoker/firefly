#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Media {
private:
  static FUNC_DEF(query);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script