#include "script/LuaModule_System.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "runtime/ModLoader.hpp"
#include "script/LuaValue.hpp"
#include <stdexcept>
using namespace firefly;
using namespace firefly::script;
int LuaModule_System::openLib(lua_State *state) {
  SET_FUNC(state, setWindowTitle);
  SET_FUNC(state, getWindowTitle);
  SET_FUNC(state, setWindowSize);
  SET_FUNC(state, getWindowSize);
  SET_FUNC(state, getMods);
  return LuaModule::openLib(state);
}
LUA_CFUNC_DEFINE(LuaModule_System::getWindowTitle) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  auto title = win->getTitle();
  return {LuaValue::create(state, title)};
}
LUA_CFUNC_DEFINE(LuaModule_System::setWindowTitle) {
  if (!LuaValue::validate<LUA_TSTRING>(args)) {
    throw std::runtime_error(
        fmt::format("Failed to set window title: cannot convert '{}' to '{}'",
                    LuaValue::stackTypesToString(args),
                    LuaValue::stackTypesToString<LUA_TSTRING>()));
  }
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  auto title = args[0]->toString();
  win->setTitle(title);
  return {};
}
LUA_CFUNC_DEFINE(LuaModule_System::getWindowSize) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  auto size = win->getSize();
  return {LuaValue::create(state, size.x), LuaValue::create(state, size.y)};
}
LUA_CFUNC_DEFINE(LuaModule_System::setWindowSize) {
  if (!LuaValue::validate<LUA_TNUMBER, LUA_TNUMBER>(args)) {
    throw std::runtime_error(
        fmt::format("Failed to set window title: cannot convert '{}' to '{}'",
                    LuaValue::stackTypesToString(args),
                    LuaValue::stackTypesToString<LUA_TNUMBER, LUA_TNUMBER>()));
  }
  auto x = args[0]->toInteger();
  auto y = args[1]->toInteger();
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  win->setSize({x, y});
  return {};
}
LUA_CFUNC_DEFINE(LuaModule_System::getMods) {
  auto mod = core::Singleton<runtime::ModLoader>::instance();
  auto mods = mod->getMods();
  auto result = LuaValue::create(state, LuaValue::LuaRawObject{});
  auto i = 1;
  for (auto &[_, mod] : mods) {
    auto manifest = LuaValue::create(state, LuaValue::LuaRawObject{});
    manifest->setField("name", LuaValue::create(state, mod.name));
    manifest->setField("displayName", LuaValue::create(state, mod.displayName));
    manifest->setField("version",
                       LuaValue::create(state, mod.version.toString()));
    manifest->setField("icon", LuaValue::create(state, mod.icon));
    manifest->setField("preview", LuaValue::create(state, mod.preview));
    manifest->setField("description", LuaValue::create(state, mod.description));
    manifest->setField("author", LuaValue::create(state, mod.author));
    manifest->setField("email", LuaValue::create(state, mod.email));
    manifest->setField("loaded", LuaValue::create(state, mod.loaded));
    auto dependences = LuaValue::create(state, LuaValue::LuaRawObject{});
    auto j = 1;
    for (auto &dep : mod.dependences) {
      auto d = LuaValue::create(state, LuaValue::LuaRawObject{});
      d->setField("name", LuaValue::create(state, dep.first));
      d->setField("version", LuaValue::create(state, dep.second.toString()));
      dependences->setIndex(j, d);
      j++;
    }
    manifest->setField("dependences", dependences);
    auto runtimeDependences = LuaValue::create(state, LuaValue::LuaRawObject{});
    j = 0;
    for (auto &dep : mod.runtimeDependences) {
      auto d = LuaValue::create(state, LuaValue::LuaRawObject{});
      d->setField("name", LuaValue::create(state, dep.first));
      d->setField("version", LuaValue::create(state, dep.second.toString()));
      dependences->setIndex(j, d);
      j++;
    }
    manifest->setField("runtimeDependences", runtimeDependences);
    result->setIndex(i, manifest);
    i++;
  }
  return {result};
}