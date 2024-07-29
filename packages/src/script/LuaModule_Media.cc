#include "script/LuaModule_Media.hpp"
#include "core/Singleton.hpp"
#include "runtime/Media.hpp"
#include "script/LuaModule.hpp"
#include "script/LuaModule_Buffer.hpp"
#include "script/LuaValue.hpp"
#include <fmt/core.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::script;
int LuaModule_Media::openLib(lua_State *state) {
  setField("query",
           LuaValue::create(state, std::function(&LuaModule_Media::query)));
  return LuaModule::openLib(state);
}
LUA_CFUNC_DEFINE(LuaModule_Media::query) {
  if (!LuaValue::validate<LUA_TSTRING>(args)) {
    throw std::runtime_error(
        fmt::format("Failed to query resource: cannot convert '{}' to '{}'",
                    LuaValue::stackTypesToString(args),
                    LuaValue::stackTypesToString<LUA_TSTRING>()));
  }
  auto name = args[0]->toString();
  auto media = core::Singleton<runtime::Media>::instance();
  auto resource = media->load(name);
  if (resource == nullptr) {
    return {LuaValue::create(state)};
  }
  auto buffer = resource->read();
  return LuaModule_Buffer::clazzBuffer->create(state, buffer);
}