#include "GameApplication.hpp"
#include "core/Singleton.hpp"
#include "input/Event_Click.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_MouseDown.hpp"
#include "input/Event_MouseMotion.hpp"
#include "input/Event_MouseWheel.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "runtime/Application.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "script/bridge/LuaBridge.hpp"
#include "script/helper/Trait_Buffer.hpp"
#include "script/helper/Trait_Resource.hpp"
#include "script/lib/Module_Event.hpp"
#include "script/lib/Module_Input.hpp"
#include "script/lib/Module_Locale.hpp"
#include "script/lib/Module_Log.hpp"
#include "script/lib/Module_Media.hpp"
#include "script/lib/Module_Runtime.hpp"
#include <SDL_image.h>
#include <SDL_keycode.h>
#include <SDL_scancode.h>
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
  script::Module_Runtime::open(_script);
  script::Module_Input::open(_script);
  _script->eval("require('duskland')");
}
void GameApplication::initLocale() {
  _locale->scan();
  _locale->setDefaultLang("en_US");
  _locale->setLang("en_US");
}
void GameApplication::initEvent() {
  _eventbus->on(this, &GameApplication::onKeyDown);
  _eventbus->on(this, &GameApplication::onKeyUp);
  _eventbus->on(this, &GameApplication::onMouseMotion);
  _eventbus->on(this, &GameApplication::onMouseDown);
  _eventbus->on(this, &GameApplication::onMouseWheel);
  _eventbus->on(this, &GameApplication::onClick);
}
void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _mouse = core::Singleton<input::Mouse>::instance();
  _keyboard = core::Singleton<input::Keyboard>::instance();
  _media->addCurrentWorkspaceDirectory(cwd().append("media").string());
  auto res = _media->load("texture::wall.jpg");
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

void GameApplication::onKeyDown(input::Event_KeyDown &e) {
  script::Module_Event::emit(_script, "keyDown",
                             createNumber(_script, e.getScancode()));
}

void GameApplication::onKeyUp(input::Event_KeyUp &e) {
  script::Module_Event::emit(_script, "keyUp",
                             createNumber(_script, e.getScancode()));
}

void GameApplication::onMouseMotion(input::Event_MouseMotion &e) {
  auto pos = e.getPosition() - getWindow()->getWindowPosition();
  auto delta = e.getDelta();
  script::Module_Event::emit(
      _script, "mouseMotion",
      _script->createValue()
          ->setObject(_script)
          ->setField(_script, "x", createNumber(_script, pos.x))
          ->setField(_script, "y", createNumber(_script, pos.y))
          ->setField(_script, "dx", createNumber(_script, delta.x))
          ->setField(_script, "dy", createNumber(_script, delta.y)));
}

void GameApplication::onMouseDown(input::Event_MouseDown &e) {
  script::Module_Event::emit(_script, "mouseDown",
                             createNumber(_script, e.getType()));
}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {
  script::Module_Event::emit(
      _script, "mouseWheel",
      _script->createValue()
          ->setObject(_script)
          ->setField(_script, "x", createNumber(_script, e.getOffset().x))
          ->setField(_script, "y", createNumber(_script, e.getOffset().y))
          ->setField(_script, "dx", createNumber(_script, e.getDelta().x))
          ->setField(_script, "dy", createNumber(_script, e.getDelta().y)));
}

void GameApplication::onClick(input::Event_Click &e) {
  script::Module_Event::emit(_script, "click",
                             createNumber(_script, e.getType()));
}