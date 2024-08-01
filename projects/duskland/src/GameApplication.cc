#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Event_MouseWheel.hpp"
#include "runtime/Application.hpp"
#include "script/LuaModule_Buffer.hpp"
#include "script/LuaModule_Event.hpp"
#include "script/LuaModule_Locale.hpp"
#include "script/LuaModule_Log.hpp"
#include "script/LuaModule_Media.hpp"
#include "script/LuaModule_System.hpp"
#include "video/Sprite2D.hpp"
#include "video/Texture2D.hpp"
#include <SDL_image.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <lua.hpp>

using namespace firefly;
using namespace duskland;
core::AutoPtr<video::Sprite2D> sprite;
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv){};
void GameApplication::initScript() {
  auto ctx = _script->pushContext();
  _script->eval("package.path = package.path .. ';./mods/?/lua/init.lua'");
  _script->openLib<script::LuaModule_Log>("log");
  _script->openLib<script::LuaModule_Event>("event");
  _script->openLib<script::LuaModule_Buffer>("buffer");
  _script->openLib<script::LuaModule_Media>("media");
  _script->openLib<script::LuaModule_System>("system");
  _script->openLib<script::LuaModule_Locale>("locale");
  _script->eval("require '.'");
  _script->popContext(ctx);
}
void GameApplication::initLocale() {
  _locale->scan();
  _locale->setDefaultLang("en_US");
  _locale->setLang("en_US");
  _script->gc(true);
}
void GameApplication::initEvent() {
  _eventbus->on(this, &GameApplication::onMouse);
  _eventbus->on(this, &GameApplication::onKeydown);
  _eventbus->on(this, &GameApplication::onMouseButtonDown);
  _eventbus->on(this, &GameApplication::onMouseWheel);
}
void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _media->addCurrentWorkspaceDirectory(cwd().append("media").string());
  initEvent();
  initLocale();
  initScript();
  _mod->loadAll(cwd().append("mods").string());
  _locale->reload();
  _script->emit("gameLoaded");
  _renderer->initDefaultResourceSet();
  _renderer->setClearColor(0.2, 0.3, 0.3, 1.0);
  sprite = new video::Sprite2D();
  auto tex = new video::Texture2D();
  tex->setImage(_media->load("texture::wall.jpg")->read());
  _renderer->setResourceSet({.textures = {{"wall", tex}}});
  sprite->enable();
  sprite->setTexture("wall");
  sprite->setTarget({0, 0, 512, 512});
  sprite->setSource({0, 0, 512, 512});
  getWindow()->show();
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  static auto time = SDL_GetTicks();
  auto now = SDL_GetTicks();
  if (now - time > 50) {
    time = now;
    _script->emit("tick");
    auto &rc = sprite->getTarget();
    sprite->setTarget({rc.position.x + 1, rc.position.y, rc.size});
  }
  _renderer->render();
  getWindow()->present();
}

void GameApplication::onUnInitialize() {
  runtime::Application::onUnInitialize();
}

void GameApplication::onMouse(input::Event_Mouse &e) {}

void GameApplication::onMouseButtonDown(input::Event_MouseButtonDown &e) {}

void GameApplication::onKeydown(input::Event_KeyDown &e) {}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {}