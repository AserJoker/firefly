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
#include "script/helper/Trait_Animation.hpp"
#include "script/helper/Trait_Buffer.hpp"
#include "script/helper/Trait_Node.hpp"
#include "script/helper/Trait_Promise.hpp"
#include "script/helper/Trait_Properties.hpp"
#include "script/helper/Trait_RenderTarget.hpp"
#include "script/helper/Trait_Resource.hpp"
#include "script/helper/Trait_Scene.hpp"
#include "script/helper/Trait_Sprite2D.hpp"
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
#include "video/Node.hpp"
#include "video/ParticleGenerator.hpp"
#include "video/Scene.hpp"
#include <SDL_image.h>
#include <SDL_mouse.h>
#include <SDL_timer.h>
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
#include <string>

using namespace firefly;
using namespace duskland;

class ConstantNode : public video::Node {
private:
  glm::ivec2 _position;

public:
  ConstantNode() {
    defineAttribute("cursor.x", &_position.x);
    defineAttribute("cursor.y", &_position.y);
  }
  void setPosition(const glm::ivec2 &cursor) {
    beginAttrGroup("cursor");
    setAttribute("x", cursor.x);
    setAttribute("y", cursor.y);
    endAttrGroup();
  }
};

core::AutoPtr<video::Scene> scene;
core::AutoPtr<video::ParticleGenerator> pg;
core::AutoPtr<ConstantNode> cursor;

GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv){};

void GameApplication::initScript() {
  _script->setBridge(new script::LuaBridge());
  _script->eval(
      "package.path = package.path..';./mods/?/lua/init.lua;./lua/?/init.lua'");
  auto scope = _script->pushScope();
  script::Trait_Buffer::initialize(_script);
  script::Trait_Resource::initialize(_script);
  script::Trait_Properties::initialize(_script);
  script::Trait_Promise::initialize(_script);
  script::Trait_Node::initialize(_script);
  script::Trait_Sprite2D::initialize(_script);
  script::Trait_Scene::initialize(_script);
  script::Trait_RenderTarget::initialize(_script);
  script::Trait_Animation::initialize(_script);

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
  _script->popScope(scope);
  _script->gc();
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
  glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
  getWindow()->show();
  srand((unsigned)time(NULL));
  scene = new video::Scene();
  pg = new video::ParticleGenerator(1000);
  pg->setTexture("star.png");
  pg->setScale({9, 9});
  pg->setRandomAngle(60);
  pg->setSpeed(5.0f);
  pg->setPosition({100, 100});
  pg->setRadialAcceleration(0.01f);
  // pg->setTangentialAcceleration(0.1f);
  pg->setLocalCoord(false);
  pg->setLifetime(4);
  scene->appendChild(pg);
  cursor = new ConstantNode();
  scene->appendChild(cursor);
  pg->bindAttribute("position", cursor, "cursor");
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  static auto time = SDL_GetTicks64();
  static auto timePreFrame = SDL_GetTicks64();
  auto now = SDL_GetTicks64();
  if (now - time > 50) {
    time = now;
    script::Module_Event::emit(_script, "tick");
  }
  script::Module_Event::emit(_script, "update", (uint32_t)(now - timePreFrame));
  timePreFrame = now;
  // if (video::Scene::scene != nullptr) {
  //   video::Scene::scene->onTick();
  // }
  if (scene != nullptr) {
    scene->onTick();
  }
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
  script::Module_Event::emit(_script, "keyDown", e.getScancode());
}

void GameApplication::onKeyUp(input::Event_KeyUp &e) {
  script::Module_Event::emit(_script, "keyUp", e.getScancode());
}

void GameApplication::onMouseMotion(input::Event_MouseMotion &e) {
  auto pos = e.getPosition();
  auto delta = e.getDelta();
  script::Module_Event::emit(
      _script, "mouseMotion",
      script::AnyRecord{
          {"x", pos.x}, {"y", pos.y}, {"dx", delta.x}, {"dy", delta.y}});
  cursor->setPosition(pos);
}

void GameApplication::onMouseDown(input::Event_MouseDown &e) {
  script::Module_Event::emit(_script, "mouseDown", e.getType());
}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {
  script::Module_Event::emit(_script, "mouseWheel",
                             script::AnyRecord{{"x", e.getOffset().x},
                                               {"y", e.getOffset().y},
                                               {"dx", e.getDelta().x},
                                               {"dy", e.getDelta().y}});
}

void GameApplication::onClick(input::Event_Click &e) {
  script::Module_Event::emit(_script, "click", e.getType());
  for (auto i = 0; i < 10; i++) {
    pg->generateParticle();
  }
}

void GameApplication::onResize(runtime::Event_Resize &e) {
  _renderer->setViewport({0, 0, e.getSize()});
  script::Module_Event::emit(
      _script, "resize",
      script::AnyRecord{{"width", e.getSize().x}, {"height", e.getSize().y}});
}