#include "script/LuaScript.hpp"
#include "core/AutoPtr.hpp"
#include "script/LuaValue.hpp"
#include <fmt/core.h>
#include <lua.h>
#include <lua.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
using namespace firefly::script;
LuaScript::LuaScript() : _state(nullptr) {
  _state = luaL_newstate();
  luaL_openlibs(_state);
}
LuaScript::~LuaScript() { lua_close(_state); }
void LuaScript::initialize() {
  auto ctx = pushContext();
  auto global = LuaValue::getGlobal(_state);
  createObjectChain({"_NATIVE"});
  popContext(ctx);
}
void LuaScript::createObjectChain(const std::vector<std::string> &chain) {
  auto ctx = pushContext();
  auto global = LuaValue::getGlobal(_state);
  auto obj = global;
  for (auto &field : chain) {
    auto item = obj->getField(field);
    if (item->getType() == LUA_TNIL) {
      item = LuaValue::create(_state, LuaValue::LuaRawObject());
      obj->setField(field, item);
    }
    obj = item;
  }
  popContext(ctx);
}
int LuaScript::pushContext() { return lua_gettop(_state); }
void LuaScript::popContext(int ctx) { lua_settop(_state, ctx); }
void LuaScript::eval(const std::string &source) {
  if (luaL_dostring(_state, source.c_str())) {
    auto err = lua_tostring(_state, -1);
    throw std::runtime_error(
        fmt::format("Failed to exec lua script:\n\t{}", err));
  }
}
lua_State *LuaScript::getLuaContext() { return _state; }