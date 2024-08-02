#pragma once
#include "LuaModule.hpp"
#include "script/LuaValue.hpp"
namespace firefly::script {
class LuaModule_Scene : public LuaModule {
private:
  static LUA_CFUNC_DEFINE(setScene);

public:
  int openLib(lua_State *state) override;
};
}; // namespace firefly::script