#include "script/LuaClass.hpp"
#include "script/LuaValue.hpp"
using namespace firefly;
using namespace firefly::script;
LuaClass::LuaClass(lua_State *state) : LuaValue(state) {
  _constructor = [](lua_State *state,
                    const LuaValueStack &args) -> core::AutoPtr<core::Object> {
    return new core::Object();
  };
  setCFunction(
      [this](lua_State *state, const LuaValueStack &args) -> LuaValueStack {
        return this->construct(state, args);
      });
}