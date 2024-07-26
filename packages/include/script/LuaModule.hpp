#pragma once
#include "core/Object.hpp"
#include <lua.hpp>
#include <vector>
namespace firefly::script {
class LuaModule : public core::Object {
private:
  std::vector<luaL_Reg> _exports;

public:
  LuaModule();
  void openLib(lua_State *state);
  void setFunc(const char *name, int (*func)(lua_State *state));
};
}; // namespace firefly::script