#include "GameApplication.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Event_MouseWheel.hpp"
#include "runtime/Application.hpp"
#include "script/Value.hpp"
#include "script/bridge/LuaBridge.hpp"
#include "script/helper/Trait_Buffer.hpp"
#include "script/helper/Trait_Resource.hpp"
#include "script/lib/Module_Event.hpp"
#include "script/lib/Module_Locale.hpp"
#include "script/lib/Module_Log.hpp"
#include "script/lib/Module_Media.hpp"
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
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv){};
void GameApplication::initScript() {
  _script->setBridge(new script::LuaBridge());
  _script->eval(
      "package.path = package.path..';./mods/?/lua/init.lua;./lua/?/init.lua'");
  script::Trait_Buffer::initialize(_script);
  script::Trait_Resource::initialize(_script);
  script::Module_Log::open(_script);
  script::Module_Locale::open(_script);
  script::Module_Event::open(_script);
  script::Module_Media::open(_script);
  _script->eval("require('duskland')");
}
void GameApplication::initLocale() {
  _locale->scan();
  _locale->setDefaultLang("en_US");
  _locale->setLang("en_US");
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
  auto res = _media->load("texture::wall.jpg");
  _renderer->initDefaultResourceSet();
  initEvent();
  initLocale();
  initScript();
  _mod->loadAll(cwd().append("mods").string());
  _locale->reload();
  _renderer->setClearColor(0.2, 0.3, 0.3, 1.0);
  script::Module_Event::emit(_script, "gameLoaded");
  getWindow()->show();
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  static auto time = SDL_GetTicks();
  auto now = SDL_GetTicks();
  if (now - time > 50) {
    time = now;
    script::Module_Event::emit(_script, "tick");
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