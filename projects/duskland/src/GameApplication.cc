#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Event_MouseWheel.hpp"
#include "runtime/Application.hpp"
#include "script/LuaModule_Buffer.hpp"
#include "script/LuaModule_Event.hpp"
#include "script/LuaModule_Log.hpp"
#include "script/LuaModule_Media.hpp"
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
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv){};
void GameApplication::initScript() {
  auto ctx = _script->pushContext();
  _script->eval("package.path = package.path .. ';./mods/?/lua/init.lua'");
  _script->openLib<script::LuaModule_Log>("log");
  _script->openLib<script::LuaModule_Event>("event");
  _script->openLib<script::LuaModule_Buffer>("buffer");
  _script->openLib<script::LuaModule_Media>("media");
  _script->popContext(ctx);
}
void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _window = new runtime::Window("duskland", 1024, 768);
  _eventbus->on(this, &GameApplication::onMouse);
  _eventbus->on(this, &GameApplication::onKeydown);
  _eventbus->on(this, &GameApplication::onMouseButtonDown);
  _eventbus->on(this, &GameApplication::onMouseWheel);
  _media->addCurrentWorkspaceDirectory(cwd().append("media").string());
  _locale->setDefaultLang("en_US");
  _locale->setLang("zh_CN");
  initScript();
  _script->eval("require '.'");
  _mod->loadAll(cwd().append("mods").string());
  _script->gc(true);
  _locale->reload();
  _script->emit("gameLoaded");
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  _window->present();
}

void GameApplication::onUnInitialize() {
  runtime::Application::onUnInitialize();
}

void GameApplication::onMouse(input::Event_Mouse &e) {}

void GameApplication::onMouseButtonDown(input::Event_MouseButtonDown &e) {}

void GameApplication::onKeydown(input::Event_KeyDown &e) {}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {}