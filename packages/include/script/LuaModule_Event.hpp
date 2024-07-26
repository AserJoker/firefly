#pragma once
#include "LuaModule.hpp"
namespace firefly::script {
class LuaModule_Event : public LuaModule {
public:
  void initialize() override;
};
}; // namespace firefly::script