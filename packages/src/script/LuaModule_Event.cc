#include "script/LuaModule_Event.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/EventBus.hpp"
#include "script/LuaScript.hpp"
#include "script/LuaValue.hpp"
#include <unordered_map>
using namespace firefly;
using namespace firefly::script;
LuaModule_Event::LuaModule_Event() {
  auto script = core::Singleton<script::LuaScript>::instance();
  auto context = script->pushContext();
  auto ctx = script->getLuaContext();
  auto global = LuaValue::getGlobal(ctx);
  auto native = global->getField("_NATIVE");
  native->setField(
      "event",
      LuaValue::create(
          ctx, std::unordered_map<std::string, core::AutoPtr<LuaValue>>()));
  script->popContext(context);
  auto bus = core::Singleton<runtime::EventBus>::instance();
  bus->on(this, &LuaModule_Event::onLuaEvent);
}
void LuaModule_Event::onLuaEvent(Event_Lua &e) {
  auto &type = e.getType();
  auto &arg = e.getArg();
  auto script = core::Singleton<script::LuaScript>::instance();
  auto context = script->pushContext();
  auto ctx = script->getLuaContext();
  auto global = LuaValue::getGlobal(ctx);
  auto native = global->getField("_NATIVE");
  auto event = native->getField("event");
  auto callbacks = event->getField(type);
  if (callbacks != nullptr) {
    for (auto i = 1; i <= callbacks->getLength(); i++) {
      auto cb = callbacks->getIndex(i);
      cb->call({arg});
    }
  }
  script->popContext(context);
}

int LuaModule_Event::openLib(lua_State *state) {
  setField("onEvent",
           LuaValue::create(state, std::function(&LuaModule_Event::onEvent)));
  setField("offEvent",
           LuaValue::create(state, std::function(&LuaModule_Event::offEvent)));
  return LuaModule::openLib(state);
}
LuaValue::LuaValueStack
LuaModule_Event::onEvent(lua_State *state,
                         const LuaValue::LuaValueStack &args) {
  auto type = args[0]->toString();
  auto callback = args[1];
  auto global = LuaValue::getGlobal(state);
  auto native = global->getField("_NATIVE");
  auto event = native->getField("event");
  auto callbacks = event->getField(type);
  if (callbacks->getType() == LUA_TNIL) {
    callbacks = LuaValue::create(state, std::vector<core::AutoPtr<LuaValue>>());
    event->setField(type, callbacks);
  }
  callbacks->setIndex(callbacks->getLength() + 1, callback);
  return {};
}
LuaValue::LuaValueStack
LuaModule_Event::offEvent(lua_State *state,
                          const LuaValue::LuaValueStack &args) {
  auto type = args[0]->toString();
  auto callback = args[1];

  auto global = LuaValue::getGlobal(state);
  auto native = global->getField("_NATIVE");
  auto event = native->getField("event");
  auto callbacks = event->getField(type);
  if (callbacks != LUA_TNIL) {
    auto len = callbacks->getLength();
    for (auto i = 1; i <= len; i++) {
      auto cb = callbacks->getIndex(i);
      if (cb->isEqual(callback)) {
        for (auto j = i; i <= len - 1; j++) {
          callbacks->setIndex(j, cb->getIndex(j + 1));
        }
        callbacks->setIndex(len, LuaValue::create(state));
        break;
      }
    }
  }
  return {};
}