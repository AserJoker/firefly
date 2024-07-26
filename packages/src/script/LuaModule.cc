#include "script/LuaModule.hpp"
#include "script/LuaScript.hpp"
using namespace firefly;
using namespace firefly::script;
LuaModule::LuaModule() {}
void LuaModule::openLib(lua_State *state) {
  _exports.push_back({NULL, NULL});
  lua_createtable(state, 0, _exports.size() - 1);
  luaL_setfuncs(state, _exports.data(), 0);
}
void LuaModule::setFunc(const char *name, int (*func)(lua_State *state)) {
  _exports.push_back(luaL_Reg{name, func});
}