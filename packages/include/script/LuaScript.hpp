#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Singleton.hpp"
#include "runtime/EventBus.hpp"
#include "script/Event_Lua.hpp"
#include "script/LuaValue.hpp"
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
  int pushContext();
  void popContext(int ctx);
  void eval(const std::string &source);
  void createObjectChain(const std::vector<std::string> &chain);
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
  template <class T> void emit(const std::string &type, T &&value) {
    auto context = pushContext();
    auto bus = core::Singleton<runtime::EventBus>::instance();
    bus->emit<Event_Lua>(type, LuaValue::create(_state, value));
    popContext(context);
  }
  void gc(bool full = false);
};
}; // namespace firefly::script