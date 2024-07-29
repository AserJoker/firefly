#pragma once
#include "LuaModule.hpp"
namespace firefly::script {
class LuaModule_Media : public LuaModule {
private:
  static LUA_CFUNC_DEFINE(query);

public:
  int openLib(lua_State *state) override;
};
} // namespace firefly::script