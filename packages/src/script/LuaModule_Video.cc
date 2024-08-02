#include "script/LuaModule_Video.hpp"
#include "core/Singleton.hpp"
#include "runtime/Media.hpp"
#include "script/LuaClass.hpp"
#include "script/LuaModule.hpp"
#include "script/LuaValue.hpp"
#include "video/Geometry.hpp"
#include "video/Renderer.hpp"
#include "video/Sprite2D.hpp"
#include "video/Texture2D.hpp"
#include <fmt/core.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::script;
int LuaModule_Video::openLib(lua_State *state) {
  clazzSprite2D = new LuaClass(state);
  clazzSprite2D->constructor(&LuaModule_Video::createSprite2D)
      ->method("setTexture", &Sprite2D_setTexture)
      ->method("setRect", &Sprite2D_setRect)
      ->method("setTextureRect", &Sprite2D_setTextureRect)
      ->method("setRotation", &LuaModule_Video::Sprite2D_setRotation)

      ->method("getTexture", &Sprite2D_getTexture)
      ->method("getRect", &Sprite2D_getRect)
      ->method("getTextureRect", &Sprite2D_getTextureRect)
      ->method("getRotation", &LuaModule_Video::Sprite2D_getRotation)

      ->method("enable", &LuaModule_Video::Sprite2D_enable)
      ->method("disable", &LuaModule_Video::Sprite2D_disable);
  clazzTexture2D = new LuaClass(state);

  clazzTexture2D->method("getSize", &LuaModule_Video::Texture2D_getSize);

  setField("createSprite2D", clazzSprite2D);
  setField("loadTexture", LuaValue::create(state, std::function(&loadTexture)));
  return LuaModule::openLib(state);
}

LUA_CFUNC_DEFINE(LuaModule_Video::loadTexture) {
  if (!LuaValue::validate<LUA_TSTRING, LUA_TSTRING>(args)) {
    throw std::runtime_error("Failed to load texture,args type is invalid");
  }
  auto name = args[0]->toString();
  auto path = args[1]->toString();
  auto renderer = core::Singleton<video::Renderer>::instance();
  auto media = core::Singleton<runtime::Media>::instance();
  core::AutoPtr tex = new video::Texture2D();
  tex->setImage(media->load(path)->read());
  renderer->setTexture(name, tex);
  return {};
}

LUA_CONSTRUCTOR_DEFINE(LuaModule_Video::createSprite2D) {
  return new video::Sprite2D();
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_setTexture) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TSTRING>(args)) {
    throw std::runtime_error(
        "Failed to set texture,texture name is not a string");
  }
  auto name = args[1]->toString();
  auto sprite = self.cast<video::Sprite2D>();
  sprite->setTexture(name);
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_enable) {
  auto sprite = self.cast<video::Sprite2D>();
  sprite->enable();
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_disable) {
  auto sprite = self.cast<video::Sprite2D>();
  sprite->disable();
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_setRect) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TTABLE>(args)) {
    throw std::runtime_error("Failed to set rect,rect is not a table");
  }
  auto rect = args[1];
  video::Geometry::Rect rc;
  rc.position.x = rect->getField("x")->toInteger();
  rc.position.y = rect->getField("y")->toInteger();
  rc.size.w = rect->getField("width")->toInteger();
  rc.size.h = rect->getField("height")->toInteger();
  auto sprite = self.cast<video::Sprite2D>();
  sprite->setRect(rc);
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_setTextureRect) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TTABLE>(args)) {
    throw std::runtime_error("Failed to set texture rect,rect is not a table");
  }
  auto rect = args[1];
  video::Geometry::Rect rc;
  rc.position.x = rect->getField("x")->toInteger();
  rc.position.y = rect->getField("y")->toInteger();
  rc.size.w = rect->getField("width")->toInteger();
  rc.size.h = rect->getField("height")->toInteger();
  auto sprite = self.cast<video::Sprite2D>();
  sprite->setTextureRect(rc);
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_setRotation) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TTABLE>(args)) {
    throw std::runtime_error("Failed to set texture rect,rect is not a table");
  }
  auto rotation = args[1];
  video::Geometry::Rotation rot;
  rot.center.x = rotation->getField("x")->toInteger();
  rot.center.y = rotation->getField("y")->toInteger();
  rot.center.z = rotation->getField("z")->toInteger();
  rot.angle = rotation->getField("angle")->toNumber();
  auto sprite = self.cast<video::Sprite2D>();
  sprite->setRotation(rot);
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_getTexture) {
  auto sprite = self.cast<video::Sprite2D>();
  auto &name = sprite->getTexture();
  auto renderer = core::Singleton<video::Renderer>::instance();
  auto texture = renderer->getTexture(name);
  auto tex = texture.cast<video::Texture2D>();
  return {clazzTexture2D->create(state, tex)};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_getRect) {
  auto sprite = self.cast<video::Sprite2D>();
  auto &rc = sprite->getRect();
  auto rect = LuaValue::create(state, LuaValue::LuaRawObject{});
  rect->setField("x", LuaValue::create(state, rc.position.x));
  rect->setField("y", LuaValue::create(state, rc.position.y));
  rect->setField("width", LuaValue::create(state, rc.size.w));
  rect->setField("height", LuaValue::create(state, rc.size.h));
  return {rect};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_getTextureRect) {
  auto sprite = self.cast<video::Sprite2D>();
  auto &rc = sprite->getTextureRect();
  auto rect = LuaValue::create(state, LuaValue::LuaRawObject{});
  rect->setField("x", LuaValue::create(state, rc.position.x));
  rect->setField("y", LuaValue::create(state, rc.position.y));
  rect->setField("width", LuaValue::create(state, rc.size.w));
  rect->setField("height", LuaValue::create(state, rc.size.h));
  return {rect};
}
LUA_METHOD_DEFINE(LuaModule_Video::Sprite2D_getRotation) {
  auto sprite = self.cast<video::Sprite2D>();
  auto &rot = sprite->getRotation();
  auto rotation = LuaValue::create(state, LuaValue::LuaRawObject{});
  rotation->setField("x", LuaValue::create(state, rot.center.x));
  rotation->setField("y", LuaValue::create(state, rot.center.y));
  rotation->setField("z", LuaValue::create(state, rot.center.z));
  rotation->setField("angle", LuaValue::create(state, rot.angle));
  return {rotation};
}

LUA_METHOD_DEFINE(LuaModule_Video::Texture2D_getSize) {
  auto tex = self.cast<video::Texture2D>();
  auto size = tex->getTextureSize();
  return {LuaValue::create(state, size.w), LuaValue::create(state, size.h)};
}