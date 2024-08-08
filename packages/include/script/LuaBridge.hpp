#pragma once
#include "Context.hpp"
#include "Value.hpp"
#include "core/AutoPtr.hpp"
#include <lua.hpp>
namespace firefly::script {
class LuaBridge : public Context::Bridge {
private:
  core::AutoPtr<Context> _ctx;
  lua_State *_state;

public:
  LuaBridge(core::AutoPtr<Context> ctx) : _ctx(ctx) {
    _state = luaL_newstate();
    luaL_openlibs(_state);
  }
  void dispose() override { lua_close(_state); }
  Value::Stack eval(const std::string &filename,
                    const std::string &source) override {
    return {};
  }
  core::AutoPtr<Value> getGlobal() override { return nullptr; }
  void registerModule(
      const std::string &name,
      std::unordered_map<std::string, core::AutoPtr<Value>> exports) override {}
};
} // namespace firefly::script