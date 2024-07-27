#include "script/LuaModule.hpp"
#include "core/AutoPtr.hpp"
#include "script/LuaScript.hpp"
#include "script/LuaValue.hpp"
using namespace firefly;
using namespace firefly::script;
LuaModule::LuaModule() {}
int LuaModule::openLib(lua_State *state) {
  lua_createtable(state, 0, _exports.size() - 1);
  auto idx = lua_gettop(state);
  core::AutoPtr m = new LuaValue(state, idx);
  for (auto &[key, value] : _exports) {
    m->setField(key, value);
  }
  lua_settop(state, idx);
  return 1;
}
void LuaModule::setField(const char *name,
                         const core::AutoPtr<LuaValue> &field) {
  _exports[name] = field;
}