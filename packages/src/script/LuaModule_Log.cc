#include "script/LuaModule_Log.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Logger.hpp"
#include "script/LuaValue.hpp"
using namespace firefly;
using namespace firefly::script;
int LuaModule_Log::openLib(lua_State *state) {
  lua_newtable(state);
  auto idx = lua_gettop(state);
  core::AutoPtr logger = new LuaValue(state, idx);
  LuaValue::LuaCFunction debug =
      [this](lua_State *state, const LuaValue::LuaValueStack &args) -> auto {
    return this->debug(state, args);
  };
  LuaValue::LuaCFunction info =
      [this](lua_State *state, const LuaValue::LuaValueStack &args) -> auto {
    return this->info(state, args);
  };
  LuaValue::LuaCFunction log =
      [this](lua_State *state, const LuaValue::LuaValueStack &args) -> auto {
    return this->log(state, args);
  };
  LuaValue::LuaCFunction warn =
      [this](lua_State *state, const LuaValue::LuaValueStack &args) -> auto {
    return this->warn(state, args);
  };
  LuaValue::LuaCFunction error =
      [this](lua_State *state, const LuaValue::LuaValueStack &args) -> auto {
    return this->error(state, args);
  };
  LuaValue::LuaCFunction panic =
      [this](lua_State *state, const LuaValue::LuaValueStack &args) -> auto {
    return this->panic(state, args);
  };
  logger->setField("debug", LuaValue::create(state, debug));
  logger->setField("info", LuaValue::create(state, info));
  logger->setField("log", LuaValue::create(state, log));
  logger->setField("warn", LuaValue::create(state, warn));
  logger->setField("error", LuaValue::create(state, error));
  logger->setField("panic", LuaValue::create(state, panic));
  lua_settop(state, idx);
  return 1;
}
LuaValue::LuaValueStack
LuaModule_Log::debug(lua_State *state, const LuaValue::LuaValueStack &args) {
  auto msg = args[0]->toString();
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->debug("{}", msg);
  return {};
}
LuaValue::LuaValueStack
LuaModule_Log::info(lua_State *state, const LuaValue::LuaValueStack &args) {
  auto msg = args[0]->toString();
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->info("{}", msg);
  return {};
}
LuaValue::LuaValueStack
LuaModule_Log::log(lua_State *state, const LuaValue::LuaValueStack &args) {
  auto msg = args[0]->toString();
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->log("{}", msg);
  return {};
}
LuaValue::LuaValueStack
LuaModule_Log::warn(lua_State *state, const LuaValue::LuaValueStack &args) {
  auto msg = args[0]->toString();
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->warn("{}", msg);
  return {};
}
LuaValue::LuaValueStack
LuaModule_Log::error(lua_State *state, const LuaValue::LuaValueStack &args) {
  auto msg = args[0]->toString();
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->error("{}", msg);
  return {};
}
LuaValue::LuaValueStack
LuaModule_Log::panic(lua_State *state, const LuaValue::LuaValueStack &args) {
  auto msg = args[0]->toString();
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->panic("{}", msg);
  return {};
}