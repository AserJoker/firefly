#pragma once
#include "LuaModule.hpp"
#include "script/LuaValue.hpp"
namespace firefly::script {
class LuaModule_Locale : public LuaModule {
private:
  static LUA_CFUNC_DEFINE(setLang);
  static LUA_CFUNC_DEFINE(getLang);
  static LUA_CFUNC_DEFINE(getLanguages);
  static LUA_CFUNC_DEFINE(i18n);

public:
  int openLib(lua_State *state) override;
};
}; // namespace firefly::script