#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "script/LuaValue.hpp"
#include <lua.hpp>
#include <unordered_map>
namespace firefly::script {
class LuaModule : public core::Object {
private:
  std::unordered_map<std::string, core::AutoPtr<LuaValue>> _exports;

public:
  LuaModule();
  virtual int openLib(lua_State *state);
  void setField(const char *name, const core::AutoPtr<LuaValue> &field);
  template <class T>
  void setFunction(lua_State *state, const char *name, T &&func) {
    setField(name, LuaValue::create(state, std::function(func)));
  }
};
#define SET_FUNC(state, func) setFunction(state, #func, &func)
}; // namespace firefly::script