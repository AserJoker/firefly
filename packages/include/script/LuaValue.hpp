#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <functional>
#include <lua.h>
#include <lua.hpp>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::script {
class LuaValue : public core::Object {
public:
  using LuaValueStack = std::vector<core::AutoPtr<LuaValue>>;
  using LuaCFunction =
      std::function<LuaValueStack(lua_State *state, const LuaValueStack &)>;

private:
  int _idx;
  lua_State *_state;

public:
  LuaValue(lua_State *state, int idx);
  LuaValue(lua_State *state);
  const int getType() const;
  const std::string getTypeName() const;
  void setField(const std::string &name, const core::AutoPtr<LuaValue> &value);
  core::AutoPtr<LuaValue> getField(const std::string &name);
  void setIndex(const int32_t &name, const core::AutoPtr<LuaValue> &value);
  core::AutoPtr<LuaValue> getIndex(const int32_t &name);
  std::vector<core::AutoPtr<LuaValue>>
  call(const std::vector<core::AutoPtr<LuaValue>> &args = {});
  const uint32_t getLength() const;
  const std::vector<std::string> getKeys() const;

  const std::string toString() const;
  const int32_t toInteger() const;
  const float toNumber() const;
  const bool toBoolean() const;

  void setString(const std::string &value);
  void setInteger(const int32_t &value);
  void setNumber(const float &value);
  void setBoolean(const bool &value);
  void setCFunction(lua_CFunction func);

  void setMetadata(const core::AutoPtr<LuaValue> &metadata);

  bool isEqual(const core::AutoPtr<LuaValue> &value) {
    return lua_compare(_state, _idx, value->_idx, LUA_OPEQ) == 1;
  }

private:
  inline static std::vector<LuaCFunction> _functions;

public:
  static core::AutoPtr<LuaValue> create(lua_State *state,
                                        const std::string &value);
  static core::AutoPtr<LuaValue> create(lua_State *state, const int32_t &value);
  static core::AutoPtr<LuaValue> create(lua_State *state, const float &value);
  static core::AutoPtr<LuaValue> create(lua_State *state, const bool &value);
  static core::AutoPtr<LuaValue> create(lua_State *state,
                                        const LuaCFunction &value);
  static core::AutoPtr<LuaValue> create(lua_State *state,
                                        const LuaValueStack &value);
  static core::AutoPtr<LuaValue>
  create(lua_State *state,
         const std::unordered_map<std::string, core::AutoPtr<LuaValue>> &value);

  static core::AutoPtr<LuaValue> getGlobal(lua_State *state);
};
} // namespace firefly::script