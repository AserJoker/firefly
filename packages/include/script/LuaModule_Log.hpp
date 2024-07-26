#pragma once
#include "LuaModule.hpp"
namespace firefly::script {
class LuaModule_Log : public LuaModule {
public:
  void initialize();
};
}; // namespace firefly::script