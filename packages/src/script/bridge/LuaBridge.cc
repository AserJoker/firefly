#include "script/bridge/LuaBridge.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "exception/LuaException.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include <lua.h>
#include <stdexcept>

using namespace firefly;
using namespace firefly::script;

int LuaBridge::luaFuncCall(lua_State *state) {
  auto ctx = core::Singleton<Script>::instance();
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return 0;
  }
  try {
    return bridge->call(state);
  } catch (std::runtime_error &e) {
    lua_pushstring(state, e.what());
    lua_error(state);
    return 1;
  }
}
int LuaBridge::luaFuncGC(lua_State *state) {
  auto obj = lua_gettop(state);
  lua_getfield(state, obj, "$handle");
  uint32_t handle = (uint32_t)lua_tonumber(state, -1);
  if (core::Singleton<Script>::isInitialized()) {
    auto ctx = core::Singleton<Script>::instance();
    ctx->getNativeGlobal()
        ->getField(ctx, "$functions")
        ->setIndex(ctx, handle, ctx->createValue());
  }
  return 0;
}
int LuaBridge::luaObjectGC(lua_State *state) {
  auto obj = lua_gettop(state);
  lua_getfield(state, obj, "$handle");
  uint32_t handle = (uint32_t)lua_tonumber(state, -1);
  if (core::Singleton<Script>::isInitialized()) {
    auto ctx = core::Singleton<Script>::instance();
    ctx->getNativeGlobal()
        ->getField(ctx, "$objects")
        ->setIndex(ctx, handle, ctx->createValue());
  }
  return 0;
}
int LuaBridge::luaObjectGet(lua_State *state) {
  auto obj_idx = 1;
  auto key_idx = 2;
  lua_getfield(state, obj_idx, "$handle");
  uint32_t handle = (uint32_t)lua_tonumber(state, -1);
  auto ctx = core::Singleton<Script>::instance();
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  auto self =
      ctx->getNativeGlobal()->getField(ctx, "$objects")->getIndex(ctx, handle);
  if (lua_type(state, key_idx) == LUA_TSTRING) {
    auto name = lua_tostring(state, key_idx);
    auto field = self->getField(ctx, name);
    bridge->dump(state, field);
  } else {
    auto field =
        self->getIndex(ctx, (uint32_t)lua_tonumber(state, key_idx) - 1);
    bridge->dump(state, field);
  }
  return 1;
}
int LuaBridge::luaObjectSet(lua_State *state) {
  auto obj_idx = 1;
  auto key_idx = 2;
  auto value_idx = 3;
  lua_getfield(state, obj_idx, "$handle");
  uint32_t handle = (uint32_t)lua_tonumber(state, -1);
  auto ctx = core::Singleton<Script>::instance();
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  auto self =
      ctx->getNativeGlobal()->getField(ctx, "$objects")->getIndex(ctx, handle);
  auto value = bridge->load(state, value_idx);
  if (lua_type(state, key_idx) == LUA_TSTRING) {
    self->setField(ctx, lua_tostring(state, key_idx), value);
  } else {
    self->setIndex(ctx, (uint32_t)lua_tonumber(state, key_idx) - 1, value);
  }
  return 0;
}
int LuaBridge::luaObjectLen(lua_State *state) {
  auto obj_idx = 1;
  lua_getfield(state, obj_idx, "$handle");
  uint32_t handle = (uint32_t)lua_tonumber(state, -1);
  auto ctx = core::Singleton<Script>::instance();
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  auto self =
      ctx->getNativeGlobal()->getField(ctx, "$objects")->getIndex(ctx, handle);
  lua_pushnumber(state, self->getLength(ctx));
  return 1;
}
int LuaBridge::luaObjectNext(lua_State *state) {
  auto obj_idx = 1;
  lua_getfield(state, obj_idx, "$handle");
  uint32_t handle = (uint32_t)lua_tonumber(state, -1);
  auto ctx = core::Singleton<Script>::instance();
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  auto self =
      ctx->getNativeGlobal()->getField(ctx, "$objects")->getIndex(ctx, handle);
  if (self->getType(ctx) == Atom::TYPE::ARRAY) {
    return 0;
  }
  auto keys = self->getKeys(ctx);
  if (lua_type(state, 2) == LUA_TNIL) {
    if (keys.size()) {
      lua_pushstring(state, keys[0].c_str());
      bridge->dump(state, self->getField(ctx, keys[0]));
      return 2;
    } else {
      return 0;
    }
  } else if (lua_type(state, 2) == LUA_TSTRING) {
    auto key = lua_tostring(state, 2);
    auto it = std::find(keys.begin(), keys.end(), key);
    if (it == keys.end()) {
      return 0;
    } else {
      it++;
      if (it == keys.end()) {
        return 0;
      }
      lua_pushstring(state, it->c_str());
      bridge->dump(state, self->getField(ctx, *it));
      return 2;
    }
  } else {
    return 0;
  }
}
int LuaBridge::luaObjectPairs(lua_State *state) {
  lua_pushcfunction(state, luaObjectNext);
  lua_pushvalue(state, 1);
  lua_pushnil(state);
  return 3;
}
Value::Stack LuaBridge::funcGC(core::AutoPtr<Script> ctx, Value::Stack args) {
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return {};
  }
  auto state = bridge->_state;
  auto self = args[0];
  uint32_t handle = (uint32_t)self->getRawField(ctx, "$handle")->toNumber(ctx);
  auto top = lua_gettop(state);
  lua_getglobal(state, "$functions");
  auto functions = lua_gettop(state);
  lua_pushnil(state);
  lua_seti(state, functions, handle);
  lua_settop(state, top);
  return {};
}
Value::Stack LuaBridge::funcCall(core::AutoPtr<Script> ctx, Value::Stack args) {
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return {};
  }
  auto state = bridge->_state;
  auto self = args[0];
  uint32_t handle = (uint32_t)self->getRawField(ctx, "$handle")->toNumber(ctx);
  auto top = lua_gettop(state);
  lua_getglobal(state, "$functions");
  auto functions = lua_gettop(state);
  lua_geti(state, functions, handle);
  for (size_t i = 1; i < args.size(); i++) {
    bridge->dump(state, args[i]);
  }
  auto current = lua_gettop(state);
  if (lua_pcall(state, (uint32_t)args.size() - 1, LUA_MULTRET, 0)) {
    throw exception::LuaException(lua_tostring(state, -1));
  }
  auto end = lua_gettop(state);
  Value::Stack result;
  for (auto i = current; i <= end; i++) {
    result.push_back(bridge->load(state, i));
  }
  lua_settop(state, top);
  return result;
}
Value::Stack LuaBridge::objectGC(core::AutoPtr<Script> ctx, Value::Stack args) {
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return {};
  }
  auto state = bridge->_state;
  auto self = args[0];
  uint32_t handle = (uint32_t)self->getRawField(ctx, "$handle")->toNumber(ctx);
  auto top = lua_gettop(state);
  lua_getglobal(state, "$objects");
  auto objects = lua_gettop(state);
  lua_pushnil(state);
  lua_seti(state, objects, handle);
  lua_settop(state, top);
  return {};
}
Value::Stack LuaBridge::objectGet(core::AutoPtr<Script> ctx,
                                  Value::Stack args) {
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return {};
  }
  auto state = bridge->_state;
  auto self = args[0];
  auto key = args[1];
  uint32_t handle = (uint32_t)self->getRawField(ctx, "$handle")->toNumber(ctx);
  auto top = lua_gettop(state);
  lua_getglobal(state, "$objects");
  auto objects = lua_gettop(state);
  lua_geti(state, objects, handle);
  auto obj = lua_gettop(state);
  if (key->getType(ctx) == Atom::TYPE::STRING) {
    lua_getfield(state, obj, key->toString(ctx).c_str());
  } else {
    lua_geti(state, obj, (uint32_t)key->toNumber(ctx) + 1);
  }
  auto result = bridge->load(state, lua_gettop(state));
  lua_settop(state, top);
  return {result};
}
Value::Stack LuaBridge::objectSet(core::AutoPtr<Script> ctx,
                                  Value::Stack args) {
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return {};
  }
  auto state = bridge->_state;
  auto self = args[0];
  auto key = args[1];
  auto value = args[2];
  uint32_t handle = (uint32_t)self->getRawField(ctx, "$handle")->toNumber(ctx);
  auto top = lua_gettop(state);
  lua_getglobal(state, "$objects");
  auto objects = lua_gettop(state);
  lua_geti(state, objects, handle);
  auto obj = lua_gettop(state);
  bridge->dump(state, value);
  if (key->getType(ctx) == Atom::TYPE::STRING) {
    lua_setfield(state, obj, key->toString(ctx).c_str());
  } else {
    lua_seti(state, obj, (uint32_t)key->toNumber(ctx) + 1);
  }
  lua_settop(state, top);
  return {};
}
Value::Stack LuaBridge::objectKeys(core::AutoPtr<Script> ctx,
                                   Value::Stack args) {
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return {};
  }
  auto state = bridge->_state;
  auto self = args[0];
  uint32_t handle = (uint32_t)self->getRawField(ctx, "$handle")->toNumber(ctx);
  auto top = lua_gettop(state);
  lua_getglobal(state, "$objects");
  auto objects = lua_gettop(state);
  lua_geti(state, objects, handle);
  auto obj = lua_gettop(state);
  auto result = ctx->createValue()->setArray(ctx);
  auto index = 0;
  lua_pushnil(state); /* 第一个键 */
  while (lua_next(state, obj) != 0) {
    if (lua_type(state, -2) == LUA_TSTRING) {
      result->setIndex(
          ctx, index++,
          ctx->createValue()->setString(ctx, lua_tostring(state, -2)));
    }
    lua_pop(state, 1);
  }
  lua_settop(state, top);
  return {result};
}
Value::Stack LuaBridge::objectLen(core::AutoPtr<Script> ctx,
                                  Value::Stack args) {
  auto bridge = ctx->getBridge().cast<LuaBridge>();
  if (!bridge) {
    return {};
  }
  auto state = bridge->_state;
  auto self = args[0];
  uint32_t handle = (uint32_t)self->getRawField(ctx, "$handle")->toNumber(ctx);
  auto top = lua_gettop(state);
  lua_getglobal(state, "$objects");
  auto objects = lua_gettop(state);
  lua_geti(state, objects, handle);
  auto obj = lua_gettop(state);
  lua_len(state, obj);
  auto result = ctx->createValue()->setNumber(ctx, lua_tonumber(state, -1));
  lua_settop(state, top);
  return {result};
}
LuaBridge::LuaBridge() {
  _state = luaL_newstate();
  luaL_openlibs(_state);
  lua_newtable(_state);
  lua_setglobal(_state, "$objects");
  lua_newtable(_state);
  lua_setglobal(_state, "$functions");
}
void LuaBridge::dispose() {
  if (_state) {
    lua_close(_state);
    _state = nullptr;
  }
}
Value::Stack LuaBridge::eval(const std::string &source) {
  auto top = lua_gettop(_state);
  int ret = luaL_dostring(_state, source.c_str());
  if (ret) {
    auto error = lua_tostring(_state, -1);
    throw exception::LuaException(
        fmt::format("Failed to exec script :\n\t{}", error));
  }
  auto current = lua_gettop(_state);
  Value::Stack result;
  for (auto index = top; index <= current; index++) {
    result.push_back(load(_state, index));
  }
  lua_settop(_state, top);
  lua_gc(_state, LUA_GCCOLLECT);
  return result;
}
core::AutoPtr<Value> LuaBridge::getGlobal() {
  auto scope = lua_gettop(_state);
  lua_getglobal(_state, "_G");
  auto obj = lua_gettop(_state);
  auto global = load(_state, obj);
  lua_settop(_state, scope);
  return global;
}
void LuaBridge::registerModule(const std::string &name,
                               core::AutoPtr<Value> exports) {
  auto ctx = core::Singleton<Script>::instance();
  getGlobal()
      ->getField(ctx, "package")
      ->getField(ctx, "loaded")
      ->setField(ctx, name, exports);
}
int LuaBridge::call(lua_State *state) {
  auto argc = lua_gettop(state);
  lua_getfield(state, 1, "$handle");
  uint32_t handle = (uint32_t)lua_tonumber(_state, -1);
  lua_settop(state, argc);
  auto ctx = core::Singleton<Script>::instance();
  auto func = ctx->getNativeGlobal()
                  ->getField(ctx, "$functions")
                  ->getIndex(ctx, handle);
  Value::Stack args;
  for (auto index = 2; index <= argc; index++) {
    args.push_back(load(state, index));
  }
  auto result = func->call(ctx, args);
  for (auto &item : result) {
    dump(state, item);
  }
  return (int)result.size();
}
core::AutoPtr<Value> LuaBridge::getObjectMetadata() {
  auto ctx = core::Singleton<Script>::instance();
  auto meta = ctx->createValue()->setObject(ctx);
  meta->setField(ctx, "gc", ctx->createValue()->setFunction(ctx, &objectGC));
  meta->setField(ctx, "get", ctx->createValue()->setFunction(ctx, &objectGet));
  meta->setField(ctx, "set", ctx->createValue()->setFunction(ctx, &objectSet));
  meta->setField(ctx, "keys",
                 ctx->createValue()->setFunction(ctx, &objectKeys));
  meta->setField(ctx, "length",
                 ctx->createValue()->setFunction(ctx, &objectLen));
  return meta;
}
core::AutoPtr<Value> LuaBridge::getFunctionMetadata() {
  auto ctx = core::Singleton<Script>::instance();
  auto meta = ctx->createValue()->setObject(ctx);
  meta->setField(ctx, "gc", ctx->createValue()->setFunction(ctx, &funcGC));
  meta->setField(ctx, "call", ctx->createValue()->setFunction(ctx, &funcCall));
  meta->setField(
      ctx, "typeof",
      ctx->createValue()->setFunction(
          ctx,
          [](core::AutoPtr<Script> ctx, Value::Stack args) -> Value::Stack {
            return {ctx->createValue()->setNumber(
                ctx, (uint32_t)script::Atom::TYPE::FUNCTION)};
          }));
  return meta;
}
core::AutoPtr<Value> LuaBridge::load(lua_State *state, int index) {
  auto top = lua_gettop(state);
  auto ctx = core::Singleton<Script>::instance();
  auto value = ctx->createValue();
  auto type = lua_type(state, index);
  switch (type) {
  case LUA_TNIL:
    break;
  case LUA_TBOOLEAN:
    value->setBoolean(ctx, lua_toboolean(state, index));
    break;
  case LUA_TNUMBER:
    value->setNumber(ctx, lua_tonumber(state, index));
    break;
  case LUA_TSTRING:
    value->setString(ctx, lua_tostring(state, index));
    break;
  case LUA_TTABLE: {

    lua_getfield(state, index, "$handle");
    if (lua_type(state, -1) == LUA_TNUMBER) {
      uint32_t handle = (uint32_t)lua_tonumber(state, -1);
      value = ctx->getNativeGlobal()
                  ->getField(ctx, "$objects")
                  ->getIndex(ctx, handle);
    } else {
      static int obj_idx = 0;
      value->setObject(ctx);
      value->setRawField(ctx, "$handle",
                         ctx->createValue()->setNumber(ctx, obj_idx));
      value->setMetadata(ctx, getObjectMetadata());
      lua_getglobal(state, "$objects");
      auto objects = lua_gettop(state);
      lua_pushvalue(state, index);
      lua_seti(state, objects, obj_idx);
      obj_idx++;
    }
  } break;
  case LUA_TFUNCTION: {
    static int func_idx = 0;
    value->setObject(ctx);
    value->setRawField(ctx, "$handle",
                       ctx->createValue()->setNumber(ctx, func_idx));
    value->setMetadata(ctx, getFunctionMetadata());
    lua_getglobal(state, "$functions");
    auto objects = lua_gettop(state);
    lua_pushvalue(state, index);
    lua_seti(state, objects, func_idx);
    func_idx++;
  } break;
  default:
    break;
  }
  lua_settop(state, top);
  return value;
}
void LuaBridge::dump(lua_State *state, core::AutoPtr<Value> value) {
  auto ctx = core::Singleton<Script>::instance();
  auto objects = ctx->getNativeGlobal()->getField(ctx, "$objects");
  auto functions = ctx->getNativeGlobal()->getField(ctx, "$functions");

  switch (value->getType(ctx)) {
  case Atom::TYPE::NIL:
    lua_pushnil(state);
    break;
  case Atom::TYPE::NUMBER:
    lua_pushnumber(state, value->toNumber(ctx));
    break;
  case Atom::TYPE::BOOLEAN:
    lua_pushboolean(state, value->toBoolean(ctx));
    break;
  case Atom::TYPE::STRING:
    lua_pushstring(state, value->toString(ctx).c_str());
    break;
  case Atom::TYPE::OBJECT:
  case Atom::TYPE::ARRAY: {
    static uint32_t obj_idx = 0;
    ctx->getNativeGlobal()
        ->getField(ctx, "$objects")
        ->setIndex(ctx, obj_idx, value);
    lua_newtable(state);
    auto obj = lua_gettop(_state);
    lua_pushnumber(state, obj_idx);
    lua_setfield(state, obj, "$handle");
    if (value->getType(ctx) == Atom::TYPE::ARRAY) {
      lua_pushboolean(state, true);
      lua_setfield(state, obj, "$array");
    }
    lua_newtable(state);
    auto meta = lua_gettop(state);
    lua_pushcfunction(state, luaObjectLen);
    lua_setfield(state, meta, "__len");
    lua_pushcfunction(state, luaObjectGet);
    lua_setfield(state, meta, "__index");
    lua_pushcfunction(state, luaObjectSet);
    lua_setfield(state, meta, "__newindex");
    lua_pushcfunction(state, luaObjectPairs);
    lua_setfield(state, meta, "__pairs");
    lua_pushcfunction(state, luaObjectGC);
    lua_setfield(state, meta, "__gc");
    lua_settop(state, meta);
    lua_setmetatable(_state, obj);
    obj_idx++;
  } break;
  case Atom::TYPE::FUNCTION: {
    static int func_idx = 0;
    ctx->getNativeGlobal()
        ->getField(ctx, "$functions")
        ->setIndex(ctx, func_idx, value);
    lua_newtable(state);
    auto obj = lua_gettop(_state);
    lua_pushnumber(state, func_idx);
    lua_setfield(state, obj, "$handle");
    lua_newtable(state);
    auto meta = lua_gettop(_state);
    lua_pushcfunction(state, luaFuncCall);
    lua_setfield(state, meta, "__call");
    lua_pushcfunction(state, luaFuncGC);
    lua_setfield(state, meta, "__gc");
    lua_settop(state, meta);
    lua_setmetatable(_state, obj);
    func_idx++;
  } break;
  }
}
void LuaBridge::gc() { lua_gc(_state, LUA_GCCOLLECT); }