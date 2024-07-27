#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Singleton.hpp"
#include <lua.hpp>
#include <string>
namespace firefly::script {
class LuaModule;
class LuaScript : public core::Object {
  friend class LuaModule;

private:
  lua_State *_state;

public:
  LuaScript();
  void initialize() override;
  ~LuaScript() override;
  void eval(const std::string &source);
  lua_State *getLuaContext();
  template <class Module> void openLib(const char *name) {
    luaL_requiref(
        _state, name,
        [](lua_State *state) -> int {
          core::AutoPtr<Module> module = core::Singleton<Module>::instance();
          return module->openLib(state);
        },
        1);
    lua_pop(_state, 1);
  }
};
}; // namespace firefly::script