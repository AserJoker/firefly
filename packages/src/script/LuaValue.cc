#include "script/LuaValue.hpp"
#include "core/AutoPtr.hpp"
#include <exception>
#include <lua.hpp>
#include <unordered_map>
using namespace firefly;
using namespace firefly::script;
LuaValue::LuaValue(lua_State *state) : _state(state) {
  lua_pushnil(_state);
  _idx = lua_gettop(_state);
}
LuaValue::LuaValue(lua_State *state, int idx) : _state(state), _idx(idx) {}
const int LuaValue::getType() const { return lua_type(_state, _idx); }
const std::string LuaValue::getTypeName() const {
  return lua_typename(_state, lua_type(_state, _idx));
}
void LuaValue::setField(const std::string &name,
                        const core::AutoPtr<LuaValue> &value) {
  auto top = lua_gettop(_state);
  lua_pushvalue(_state, value->_idx);
  lua_setfield(_state, _idx, name.c_str());
  lua_settop(_state, top);
}
core::AutoPtr<LuaValue> LuaValue::getField(const std::string &name) {
  lua_getfield(_state, _idx, name.c_str());
  return new LuaValue(_state, lua_gettop(_state));
}
void LuaValue::setIndex(const int32_t &name,
                        const core::AutoPtr<LuaValue> &value) {
  auto top = lua_gettop(_state);
  lua_pushvalue(_state, value->_idx);
  lua_seti(_state, _idx, name);
  lua_settop(_state, top);
}
core::AutoPtr<LuaValue> LuaValue::getIndex(const int32_t &name) {
  lua_geti(_state, _idx, name);
  return new LuaValue(_state, lua_gettop(_state));
}
std::vector<core::AutoPtr<LuaValue>>
LuaValue::call(const std::vector<core::AutoPtr<LuaValue>> &args) {
  auto top = lua_gettop(_state);
  lua_pushvalue(_state, _idx);
  for (auto &arg : args) {
    lua_pushvalue(_state, arg->_idx);
  }
  lua_call(_state, args.size(), LUA_MULTRET);
  auto now = lua_gettop(_state);
  std::vector<core::AutoPtr<LuaValue>> rets;
  for (auto i = top + 1; i <= now; i++) {
    rets.push_back(new LuaValue(_state, i));
  }
  return rets;
}
lua_State *LuaValue::getLuaContext() { return _state; }
const uint32_t LuaValue::getLength() const {
  lua_len(_state, _idx);
  uint32_t res = lua_tointeger(_state, -1);
  lua_pop(_state, 1);
  return res;
}
const std::vector<std::string> LuaValue::getKeys() const {
  auto top = lua_gettop(_state);
  std::vector<std::string> keys;
  lua_pushnil(_state);
  while (lua_next(_state, _idx) != 0) {
    if (lua_isstring(_state, -2)) {
      keys.emplace_back(lua_tostring(_state, -2));
    }
    lua_pop(_state, 1);
  }
  lua_settop(_state, top);
  return keys;
}
const std::string LuaValue::toString() const {
  return lua_tostring(_state, _idx);
}
const int32_t LuaValue::toInteger() const {
  return lua_tointeger(_state, _idx);
}
const float LuaValue::toNumber() const { return lua_tonumber(_state, _idx); }
const bool LuaValue::toBoolean() const { return lua_toboolean(_state, _idx); }
void LuaValue::setString(const std::string &value) {
  lua_pushstring(_state, value.c_str());
  lua_replace(_state, _idx);
}
void LuaValue::setInteger(const int32_t &value) {
  lua_pushinteger(_state, value);
  lua_replace(_state, _idx);
}
void LuaValue::setNumber(const float &value) {
  lua_pushnumber(_state, value);
  lua_replace(_state, _idx);
}
void LuaValue::setBoolean(const bool &value) {
  lua_pushboolean(_state, value);
  lua_replace(_state, _idx);
}
void LuaValue::setCFunction(LuaCFunction func) {
  LuaValue::_functions.push_back(func);
  lua_pushinteger(_state, LuaValue::_functions.size() - 1);
  lua_pushcclosure(
      _state,
      [](lua_State *state) -> int {
        try {
          auto idx = lua_upvalueindex(1);
          auto index = lua_tointeger(state, idx);
          auto &func = LuaValue::_functions[index];
          std::vector<core::AutoPtr<LuaValue>> args;
          for (auto i = 1; i <= lua_gettop(state); i++) {
            args.push_back(new LuaValue(state, i));
          }
          auto res = func(state, args);
          for (auto &r : res) {
            lua_pushvalue(state, r->_idx);
          }
          return res.size();
        } catch (std::exception &e) {
          luaL_error(state, e.what());
          return 0;
        }
      },
      1);
  lua_replace(_state, _idx);
}
void LuaValue::setMetadata(const core::AutoPtr<LuaValue> &metadata) {
  lua_pushvalue(_state, metadata->_idx);
  lua_setmetatable(_state, _idx);
}
core::AutoPtr<LuaValue> LuaValue::create(lua_State *state) {
  lua_pushnil(state);
  return new LuaValue(state, lua_gettop(state));
}
core::AutoPtr<LuaValue> LuaValue::create(lua_State *state,
                                         const std::string &value) {
  lua_pushstring(state, value.c_str());
  return new LuaValue(state, lua_gettop(state));
}
core::AutoPtr<LuaValue> LuaValue::create(lua_State *state, double value) {
  lua_pushnumber(state, value);
  return new LuaValue(state, lua_gettop(state));
}
core::AutoPtr<LuaValue> LuaValue::create(lua_State *state, bool value) {
  lua_pushboolean(state, value);
  return new LuaValue(state, lua_gettop(state));
}
bool LuaValue::isEqual(const core::AutoPtr<LuaValue> &value) {
  return lua_compare(_state, _idx, value->_idx, LUA_OPEQ) == 1;
}
bool LuaValue::isLess(const core::AutoPtr<LuaValue> &value) {
  return lua_compare(_state, _idx, value->_idx, LUA_OPLT) == 1;
}
bool LuaValue::isLessAndEqual(const core::AutoPtr<LuaValue> &value) {
  return lua_compare(_state, _idx, value->_idx, LUA_OPLE) == 1;
}
core::AutoPtr<LuaValue> LuaValue::create(lua_State *state,
                                         const LuaCFunction &value) {
  LuaValue::_functions.push_back(value);
  lua_pushinteger(state, LuaValue::_functions.size() - 1);
  lua_pushcclosure(
      state,
      [](lua_State *state) -> int {
        try {
          auto idx = lua_upvalueindex(1);
          auto index = lua_tointeger(state, idx);
          auto &func = LuaValue::_functions[index];
          std::vector<core::AutoPtr<LuaValue>> args;
          for (auto i = 1; i <= lua_gettop(state); i++) {
            args.push_back(new LuaValue(state, i));
          }
          auto res = func(state, args);
          for (auto &r : res) {
            lua_pushvalue(state, r->_idx);
          }
          return res.size();
        } catch (std::exception &e) {
          luaL_error(state, e.what());
          return 0;
        }
      },
      1);
  return new LuaValue(state, lua_gettop(state));
}
core::AutoPtr<LuaValue> LuaValue::create(lua_State *state,
                                         const LuaValueStack &stack) {
  lua_newtable(state);
  auto idx = lua_gettop(state);
  for (auto i = 0; i < stack.size(); i++) {
    lua_pushvalue(state, stack[i]->_idx);
    lua_seti(state, idx, i);
  }
  return new LuaValue(state, idx);
}
core::AutoPtr<LuaValue> LuaValue::create(
    lua_State *state,
    const std::unordered_map<std::string, core::AutoPtr<LuaValue>> &stack) {
  lua_newtable(state);
  auto idx = lua_gettop(state);
  for (auto &[key, value] : stack) {
    lua_pushvalue(state, value->_idx);
    lua_setfield(state, idx, key.c_str());
  }
  return new LuaValue(state, idx);
}
core::AutoPtr<LuaValue> LuaValue::getGlobal(lua_State *state) {
  lua_getglobal(state, "_G");
  return new LuaValue(state, lua_gettop(state));
}
const int32_t &LuaValue::getIndex() const { return _idx; }
const uint32_t LuaValue::storeObject(const core::AutoPtr<core::Object> &obj) {
  static uint32_t idx = 0;
  LuaValue::_storages[idx] = obj;
  return _idx++;
}

core::AutoPtr<LuaValue> LuaValue::create(lua_State *state, void *value) {
  lua_pushlightuserdata(state, value);
  return new LuaValue(state, lua_gettop(state));
}

core::AutoPtr<core::Object> LuaValue::getObject(const uint32_t &idx) {
  if (_storages.contains(idx)) {
    return _storages.at(idx);
  }
  return nullptr;
}
void LuaValue::deleteObject(const uint32_t &idx) { _storages.erase(idx); }