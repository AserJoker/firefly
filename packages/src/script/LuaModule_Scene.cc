#include "script/LuaModule_Scene.hpp"
#include "script/LuaModule.hpp"
#include "script/LuaModule_Event.hpp"
#include "script/LuaScript.hpp"
#include "script/LuaValue.hpp"
using namespace firefly;
using namespace firefly::script;
int LuaModule_Scene::openLib(lua_State *state) {
  setFunction(state, "setScene", &LuaModule_Scene::setScene);
  return LuaModule::openLib(state);
}
LUA_CFUNC_DEFINE(LuaModule_Scene::setScene) {
  auto func = args[0];
  auto result = func->call();
  if (result.size()) {
    auto scene = result[0];
    auto keys = scene->getKeys();
    for (auto &key : keys) {
      LuaModule_Event::onEvent(
          state, {LuaValue::create(state, key), scene->getField(key)});
    }
    auto global = LuaValue::getGlobal(state);
    global->setField("_SCENE", scene);
  }
  return {};
}