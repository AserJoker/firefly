#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <functional>
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

  using LuaRawObject = std::unordered_map<std::string, core::AutoPtr<LuaValue>>;

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
  lua_State *getLuaContext();
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
  void setCFunction(LuaCFunction func);

  void setMetadata(const core::AutoPtr<LuaValue> &metadata);

  bool isEqual(const core::AutoPtr<LuaValue> &value);
  bool isLess(const core::AutoPtr<LuaValue> &value);
  bool isLessAndEqual(const core::AutoPtr<LuaValue> &value);
  const int &getIndex() const;

private:
  inline static std::vector<LuaCFunction> _functions;
  inline static std::unordered_map<uint32_t, core::AutoPtr<core::Object>>
      _storages;

public:
  static core::AutoPtr<LuaValue> create(lua_State *state);
  static core::AutoPtr<LuaValue> create(lua_State *state,
                                        const std::string &value);
  static core::AutoPtr<LuaValue> create(lua_State *state, double value);
  static core::AutoPtr<LuaValue> create(lua_State *state, bool value);
  static core::AutoPtr<LuaValue> create(lua_State *state,
                                        const LuaCFunction &value);
  static core::AutoPtr<LuaValue> create(lua_State *state,
                                        const LuaValueStack &value);
  static core::AutoPtr<LuaValue>
  create(lua_State *state,
         const std::unordered_map<std::string, core::AutoPtr<LuaValue>> &value);
  static core::AutoPtr<LuaValue> create(lua_State *state, void *value);

  static core::AutoPtr<LuaValue> getGlobal(lua_State *state);
  const uint32_t storeObject(const core::AutoPtr<core::Object> &obj);
  core::AutoPtr<core::Object> getObject(const uint32_t &idx);
  void deleteObject(const uint32_t &idx);

public:
  template <int32_t... types> static bool validate(const LuaValueStack &args) {
    return validateArg(args, 0, types...);
  }
  static bool validateArg(const LuaValueStack &args, int32_t index,
                          int32_t type, auto... types) {
    return args.size() > index && args[index]->getType() == type &&
           validateArg(args, index + 1, types...);
  }
  static bool validateArg(const LuaValueStack &args, int32_t index,
                          int32_t type) {
    return args.size() > index && args[index]->getType() == type;
  }
  static constexpr std::string typeToString(int32_t type) {
    switch (type) {
    case LUA_TNIL:
      return "nil";
    case LUA_TBOOLEAN:
      return "boolean";
    case LUA_TLIGHTUSERDATA:
      return "lightuserdata";
    case LUA_TNUMBER:
      return "number";
    case LUA_TSTRING:
      return "string";
    case LUA_TTABLE:
      return "table";
    case LUA_TFUNCTION:
      return "function";
    case LUA_TUSERDATA:
      return "userdata";
    case LUA_TTHREAD:
      return "thread";
    }
    return "unknown";
  }
  template <int32_t... types> static std::string stackTypesToString() {
    std::vector parts({typeToString(types)...});
    std::string result;
    for (auto &part : parts) {
      if (!result.empty()) {
        result += ",";
      }
      result += part;
    }
    return result;
  }
  static std::string stackTypesToString(const LuaValue::LuaValueStack &args) {
    std::vector<std::string> parts;
    for (auto &arg : args) {
      parts.push_back(arg->getTypeName());
    }
    std::string result;
    for (auto &part : parts) {
      if (!result.empty()) {
        result += ",";
      }
      result += part;
    }
    return result;
  }
};
#define LUA_CFUNC_DEFINE(name)                                                 \
  LuaValue::LuaValueStack name(lua_State *state,                               \
                               const LuaValue::LuaValueStack &args)
} // namespace firefly::script