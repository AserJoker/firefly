#pragma once
#include "LuaModule.hpp"
#include "script/LuaValue.hpp"
namespace firefly::script {
class LuaModule_System : public LuaModule {
private:
  static LUA_CFUNC_DEFINE(setWindowTitle);
  static LUA_CFUNC_DEFINE(getWindowTitle);
  static LUA_CFUNC_DEFINE(getWindowSize);
  static LUA_CFUNC_DEFINE(setWindowSize);
  static LUA_CFUNC_DEFINE(getMods);

public:
  int openLib(lua_State *state) override;
};
}; // namespace firefly::script