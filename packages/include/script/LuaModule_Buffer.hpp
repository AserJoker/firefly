#pragma once
#include "LuaModule.hpp"
#include "core/AutoPtr.hpp"
#include "script/LuaClass.hpp"
namespace firefly::script {
class LuaModule_Buffer : public LuaModule {
private:
  static LUA_CONSTRUCTOR_DEFINE(construct);
  static LUA_METHOD_DEFINE(getLength);
  static LUA_METHOD_DEFINE(writeUint8);
  static LUA_METHOD_DEFINE(writeUint16);
  static LUA_METHOD_DEFINE(writeUint32);
  static LUA_METHOD_DEFINE(readUint8);
  static LUA_METHOD_DEFINE(readUint16);
  static LUA_METHOD_DEFINE(readUint32);
  static LUA_METHOD_DEFINE(toUint8Array);
  static LUA_METHOD_DEFINE(toUint16Array);
  static LUA_METHOD_DEFINE(toUint32Array);
  static LUA_METHOD_DEFINE(toString);

private:
public:
  int openLib(lua_State *state) override;

public:
  static inline core::AutoPtr<LuaClass> clazzBuffer;
};
} // namespace firefly::script