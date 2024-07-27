#include "script/LuaScript.hpp"
#include "core/AutoPtr.hpp"
#include "script/LuaValue.hpp"
#include <fmt/core.h>
#include <lua.hpp>
#include <stdexcept>
#include <unordered_map>
using namespace firefly::script;
LuaScript::LuaScript() : _state(nullptr) {
  _state = luaL_newstate();
  luaL_openlibs(_state);
}
LuaScript::~LuaScript() { lua_close(_state); }
void LuaScript::initialize() {
  pushContext();
  auto global = LuaValue::getGlobal(_state);
  global->setField(
      "_NATIVE",
      LuaValue::create(
          _state, std::unordered_map<std::string, core::AutoPtr<LuaValue>>()));
  popContext();
}
void LuaScript::pushContext() { _stacktop = lua_gettop(_state); }
void LuaScript::popContext() { lua_settop(_state, _stacktop); }
void LuaScript::eval(const std::string &source) {
  if (luaL_dostring(_state, source.c_str())) {
    auto err = lua_tostring(_state, -1);
    throw std::runtime_error(
        fmt::format("Failed to exec lua script:\n\t{}", err));
  }
}
lua_State *LuaScript::getLuaContext() { return _state; }