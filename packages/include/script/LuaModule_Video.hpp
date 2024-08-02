#pragma once
#include "LuaModule.hpp"
#include "core/AutoPtr.hpp"
#include "script/LuaClass.hpp"
namespace firefly::script {
class LuaModule_Video : public LuaModule {
public:
  static inline core::AutoPtr<LuaClass> clazzSprite2D;
  static inline core::AutoPtr<LuaClass> clazzTexture2D;
  static inline core::AutoPtr<LuaClass> clazzShader;
  static inline core::AutoPtr<LuaClass> clazzMesh;

  static LUA_CONSTRUCTOR_DEFINE(createSprite2D);

  static LUA_METHOD_DEFINE(Sprite2D_setTexture);
  static LUA_METHOD_DEFINE(Sprite2D_setRect);
  static LUA_METHOD_DEFINE(Sprite2D_setTextureRect);
  static LUA_METHOD_DEFINE(Sprite2D_setRotation);
  static LUA_METHOD_DEFINE(Sprite2D_getTexture);
  static LUA_METHOD_DEFINE(Sprite2D_getRect);
  static LUA_METHOD_DEFINE(Sprite2D_getTextureRect);
  static LUA_METHOD_DEFINE(Sprite2D_getRotation);
  static LUA_METHOD_DEFINE(Sprite2D_enable);
  static LUA_METHOD_DEFINE(Sprite2D_disable);

  static LUA_METHOD_DEFINE(Texture2D_getSize);

  static LUA_CFUNC_DEFINE(loadTexture);

public:
  int openLib(lua_State *state) override;
};
} // namespace firefly::script