#include "script/LuaModule_Locale.hpp"
#include "core/Singleton.hpp"
#include "runtime/Locale.hpp"
#include "script/LuaModule.hpp"
#include "script/LuaValue.hpp"
#include <fmt/format.h>
using namespace firefly;
using namespace firefly::script;
int LuaModule_Locale::openLib(lua_State *state) {
  SET_FUNC(state, getLang);
  SET_FUNC(state, setLang);
  SET_FUNC(state, getLanguages);
  SET_FUNC(state, i18n);
  return LuaModule::openLib(state);
}
LUA_CFUNC_DEFINE(LuaModule_Locale::setLang) {
  if (!LuaValue::validate<LUA_TSTRING>(args)) {
    throw std::runtime_error(
        fmt::format("Failed to set language: cannot convert '{}' to '{}'",
                    LuaValue::stackTypesToString(args),
                    LuaValue::stackTypesToString<LUA_TSTRING>()));
  }
  auto lang = args[0]->toString();
  auto locale = core::Singleton<runtime::Locale>::instance();
  locale->setLang(lang);
  return {};
}
LUA_CFUNC_DEFINE(LuaModule_Locale::getLang) {
  auto locale = core::Singleton<runtime::Locale>::instance();
  return {LuaValue::create(state, locale->getLang())};
}
LUA_CFUNC_DEFINE(LuaModule_Locale::getLanguages) {
  auto locale = core::Singleton<runtime::Locale>::instance();
  auto languages = locale->getLanguages();
  auto result = LuaValue::create(state, LuaValue::LuaRawObject{});
  for (auto i = 0; i < languages.size(); i++) {
    result->setIndex(i + 1, LuaValue::create(state, languages[i]));
  }
  return {result};
}
LUA_CFUNC_DEFINE(LuaModule_Locale::i18n) {
  std::string key;
  if (LuaValue::validate<LUA_TSTRING>(args)) {
    key = args[0]->toString();
  }
  auto locale = core::Singleton<runtime::Locale>::instance();
  return {LuaValue::create(state, locale->i18n(key))};
}