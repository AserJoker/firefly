#include "script/LuaModule_System.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "script/LuaValue.hpp"
#include <stdexcept>
using namespace firefly;
using namespace firefly::script;
int LuaModule_System::openLib(lua_State *state) {
  SET_FUNC(state, setWindowTitle);
  SET_FUNC(state, getWindowTitle);
  SET_FUNC(state, setWindowSize);
  SET_FUNC(state, getWindowSize);
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