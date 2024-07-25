#pragma once
#include "core/Object.hpp"
#include <lua.hpp>
#include <string>
namespace firefly::script {
class LuaScript : public core::Object {
private:
  lua_State *_state;

public:
  LuaScript();
  ~LuaScript() override;
  void eval(const std::string &source);
};
}; // namespace firefly::script