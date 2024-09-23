#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
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
#include "script/helper/Trait_Promise.hpp"
#include "script/helper/Trait_Properties.hpp"
#include "script/helper/Trait_Resource.hpp"
#include "script/lib/Module_Array.hpp"
#include "script/lib/Module_Co.hpp"
#include "script/lib/Module_Database.hpp"
#include "script/lib/Module_Event.hpp"
#include "script/lib/Module_Input.hpp"
#include "script/lib/Module_Locale.hpp"
#include "script/lib/Module_Log.hpp"
#include "script/lib/Module_Media.hpp"
#include "script/lib/Module_Runtime.hpp"
#include "script/lib/Module_Serialization.hpp"
#include "script/lib/Module_Video.hpp"
#include "video/Camera.hpp"
#include "video/OrthoCamera.hpp"
#include "video/Renderer.hpp"
#include "video/Sprite2D.hpp"
#include <SDL_image.h>
#include <fmt/format.h>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <lua.hpp>

using namespace firefly;
using namespace duskland;
core::AutoPtr<video::Camera> camera;
core::AutoPtr<video::Sprite2D> sprite;
int32_t xframe = 0;
int32_t yframe = 0;
float angle = 0.0f;
uint32_t dx = 0;
uint32_t dy = 0;
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv){};

void GameApplication::initScript() {
  _script->setBridge(new script::LuaBridge());
  _script->eval(
      "package.path = package.path..';./mods/?/lua/init.lua;./lua/?/init.lua'");
  script::Trait_Buffer::initialize(_script);
  script::Trait_Resource::initialize(_script);
  script::Trait_Properties::initialize(_script);
  script::Trait_Promise::initialize(_script);
  script::Module_Log::open(_script);
  script::Module_Locale::open(_script);
  script::Module_Event::open(_script);
  script::Module_Media::open(_script);
  script::Module_Runtime::open(_script);
  script::Module_Input::open(_script);
  script::Module_Database::open(_script);
  script::Module_Serialization::open(_script);
  script::Module_Array::open(_script);
  script::Module_Video::open(_script);
  script::Module_Co::open(_script);
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
  _eventbus->on(this, &GameApplication::onResize);
}

void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _mouse = core::Singleton<input::Mouse>::instance();
  _keyboard = core::Singleton<input::Keyboard>::instance();
  initEvent();
  initLocale();
  initScript();
  _mod->loadAll(cwd().append("mods").string());
  _locale->reload();
  _database->load();
  script::Module_Event::emit(_script, "gameLoaded");
  sprite = new video::Sprite2D("001-Fighter01.png");
  sprite->setRect({0, 0, 64, 96});
  sprite->setSourceRect({0, 0, 32, 48});
  _renderer->setShader("2d");
  camera = new video::OrthoCamera({0.0f, 0.f, getWindow()->getSize()});
  glClearColor(0.2, 0.3, 0.2, 1.0f);
  getWindow()->setSwapInterval(0);
  getWindow()->show();
}
bool show_demo_window = true;
void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  static auto time = SDL_GetTicks();
  auto now = SDL_GetTicks();
  if (_keyboard->getKeyState(SDL_SCANCODE_A)) {
    dx = -1;
  } else if (_keyboard->getKeyState(SDL_SCANCODE_D)) {
    dx = 1;
  } else {
    dx = 0;
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_W)) {
    dy = 1;
  } else if (_keyboard->getKeyState(SDL_SCANCODE_S)) {
    dy = -1;
  } else {
    dy = 0;
  }
  if (now - time > 100) {
    time = now;
    script::Module_Event::emit(_script, "tick");
    if (dy == 1) {
      yframe = 3;
    }
    if (dy == -1) {
      yframe = 0;
    }
    if (dx == 1) {
      yframe = 2;
    }
    if (dx == -1) {
      yframe = 1;
    }
    if (dx != 0 || dy != 0) {
      xframe++;
      if (xframe == 4) {
        xframe = 0;
      }
    } else {
      xframe = 0;
    }
    sprite->setSourceRect({xframe * 32, yframe * 48, 32, 48});
  }
  script::Module_Event::emit(_script, "update");
  _renderer->setCamera(camera);
  _renderer->present();
  getWindow()->present();
}

void GameApplication::onUnInitialize() {
  _script->dispose();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  runtime::Application::onUnInitialize();
}

void GameApplication::onKeyDown(input::Event_KeyDown &e) {
  auto scope = _script->pushScope();
  script::Module_Event::emit(_script, "keyDown",
                             createNumber(_script, e.getScancode()));
  _script->popScope(scope);
  if (e.getScancode() == SDL_SCANCODE_ESCAPE) {
    _mouse->releaseMouse();
  }
}

void GameApplication::onKeyUp(input::Event_KeyUp &e) {
  auto scope = _script->pushScope();
  script::Module_Event::emit(_script, "keyUp",
                             createNumber(_script, e.getScancode()));
  _script->popScope(scope);
}

void GameApplication::onMouseMotion(input::Event_MouseMotion &e) {
  auto scope = _script->pushScope();
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
  _script->popScope(scope);
}

void GameApplication::onMouseDown(input::Event_MouseDown &e) {
  auto scope = _script->pushScope();
  script::Module_Event::emit(_script, "mouseDown",
                             createNumber(_script, e.getType()));
  _script->popScope(scope);
  _mouse->captureMouse();
}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {
  auto scope = _script->pushScope();
  script::Module_Event::emit(
      _script, "mouseWheel",
      _script->createValue()
          ->setObject(_script)
          ->setField(_script, "x", createNumber(_script, e.getOffset().x))
          ->setField(_script, "y", createNumber(_script, e.getOffset().y))
          ->setField(_script, "dx", createNumber(_script, e.getDelta().x))
          ->setField(_script, "dy", createNumber(_script, e.getDelta().y)));
  _script->popScope(scope);
}

void GameApplication::onClick(input::Event_Click &e) {
  auto scope = _script->pushScope();
  script::Module_Event::emit(_script, "click",
                             createNumber(_script, e.getType()));
  _script->popScope(scope);
}

void GameApplication::onResize(runtime::Event_Resize &e) {
  _renderer->setViewport({0, 0, e.getSize()});
  camera->setViewport({0, 0, e.getSize()});
}