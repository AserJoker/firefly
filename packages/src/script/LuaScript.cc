#include "script/LuaScript.hpp"
#include <fmt/core.h>
#include <lauxlib.h>
#include <stdexcept>
using namespace firefly::script;
LuaScript::LuaScript() : _state(nullptr) {
  _state = luaL_newstate();
  luaL_openlibs(_state);
}
LuaScript::~LuaScript() { lua_close(_state); }
void LuaScript::eval(const std::string &source) {
  if (luaL_dostring(_state, source.c_str())) {
    auto err = lua_tostring(_state, -1);
    throw std::runtime_error(
        fmt::format("Failed to exec lua script:\n\t{}", err));
  }
}