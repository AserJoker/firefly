#pragma once
#include "Event_Lua.hpp"
#include "LuaModule.hpp"
#include "script/LuaValue.hpp"
namespace firefly::script {
class LuaModule_Event : public LuaModule {
public:
  LuaModule_Event();
  void onLuaEvent(Event_Lua &event);
  virtual int openLib(lua_State *state) override;
  static LuaValue::LuaValueStack onEvent(lua_State *state,
                                         const LuaValue::LuaValueStack &args);
  static LuaValue::LuaValueStack offEvent(lua_State *state,
                                          const LuaValue::LuaValueStack &args);
};
}; // namespace firefly::script