#pragma once
#include "LuaModule.hpp"
#include "script/LuaValue.hpp"
#include <lua.hpp>
namespace firefly::script {
class LuaModule_Log : public LuaModule {
public:
  int openLib(lua_State *state) override;
  LuaValue::LuaValueStack debug(lua_State *state,
                                const LuaValue::LuaValueStack &args);
  LuaValue::LuaValueStack info(lua_State *state,
                               const LuaValue::LuaValueStack &args);
  LuaValue::LuaValueStack log(lua_State *state,
                              const LuaValue::LuaValueStack &args);
  LuaValue::LuaValueStack warn(lua_State *state,
                               const LuaValue::LuaValueStack &args);
  LuaValue::LuaValueStack error(lua_State *state,
                                const LuaValue::LuaValueStack &args);
  LuaValue::LuaValueStack panic(lua_State *state,
                                const LuaValue::LuaValueStack &args);
};
}; // namespace firefly::script