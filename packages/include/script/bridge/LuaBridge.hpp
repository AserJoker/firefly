#pragma once
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include <fmt/core.h>
#include <lua.hpp>
#include <stdexcept>
#include <unordered_map>

namespace firefly::script {
class LuaBridge : public Script::Bridge {

private:
  static int luaFuncCall(lua_State *state);
  static int luaFuncGC(lua_State *state);
  static int luaObjectGC(lua_State *state);
  static int luaObjectGet(lua_State *state);
  static int luaObjectSet(lua_State *state);
  static Value::Stack funcGC(core::AutoPtr<Script> ctx, Value::Stack args);
  static Value::Stack funcCall(core::AutoPtr<Script> ctx, Value::Stack args);
  static Value::Stack objectGC(core::AutoPtr<Script> ctx, Value::Stack args);
  static Value::Stack objectGet(core::AutoPtr<Script> ctx, Value::Stack args);
  static Value::Stack objectSet(core::AutoPtr<Script> ctx, Value::Stack args);

private:
  lua_State *_state;

  int call(lua_State *state);
  core::AutoPtr<Value> getObjectMetadata();
  core::AutoPtr<Value> getFunctionMetadata();
  core::AutoPtr<Value> load(lua_State *state, int index);
  void dump(lua_State *state, core::AutoPtr<Value> value);

public:
  LuaBridge();
  void dispose() override;
  Value::Stack eval(const std::string &source) override;
  core::AutoPtr<Value> getGlobal() override;
  void registerModule(
      const std::string &name,
      std::unordered_map<std::string, core::AutoPtr<Value>> exports) override;
};
} // namespace firefly::script