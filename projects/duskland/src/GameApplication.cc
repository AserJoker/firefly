#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
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
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include "video/Camera.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/OrthoCamera.hpp"
#include "video/Renderer.hpp"
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

core::AutoPtr<video::Geometry> quad;
core::AutoPtr<video::Material> quadMaterial;

constexpr static const glm::vec2 quadVec[] = {{0.f, 0.0f},      {1.0f, 0.0f},
                                              {0.0f, 1.0f},   {0.0f, 1.0f},
                                              {1.0f, 1.0f}, {1.0f, 0.0f}};

constexpr static const glm::vec2 quadTex[] = {{0.0f, 0.0f}, {1.0f, 0.0f},
                                              {0.0f, 1.0f}, {0.0f, 1.0f},
                                              {1.0f, 1.0f}, {1.0f, 0.0f}};

constexpr static const uint32_t quadIndex[] = {0, 1, 2, 3, 4, 5};

glm::mat4 model =

    glm::scale(glm::mat4(1.0f), {2 / 1024.0f, 2 / 768.0f, 1.0f}) *
    glm::translate(glm::mat4(1.0f), {1024.0f / -2.f, 768.0f / 2.f, 0.0f});
core::AutoPtr<video::Camera> camera;
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

  quadMaterial = new video::Material();
  quadMaterial->setTexture("diffuse_texture", {.path = "wall.jpg"});
  quadMaterial->setDepthTest(false);
  quadMaterial->setBlend(true);
  quad = new video::Geometry();
  quad->setAttribute(
      video::Geometry::ATTR_POSITION,
      new video::Attribute(new core::Buffer(sizeof(quadVec), quadVec),
                           typeid(float), 2));
  quad->setAttribute(
      video::Geometry::ATTR_TEXCOORD,
      new video::Attribute(new core::Buffer(sizeof(quadTex), quadTex),
                           typeid(float), 2));
  quad->setAttributeIndex(new video::AttributeIndex(
      new core::Buffer(sizeof(quadIndex), quadIndex)));
  auto size = getWindow()->getSize();
  _renderer->setShader("2d");
  camera = new video::OrthoCamera({0.0f, 0.f, 1024.0f / 2, 768.0f / 2});
  auto pos = camera->getProjectionMatrix() *
             glm::vec4(100.0f / 1024, 100.f / 768, 0.f, 1.f);
  auto x = pos.x * 1024;
  auto y = pos.y * 768;
  getWindow()->setSwapInterval(0);
  getWindow()->show();
}
bool show_demo_window = true;
void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  static auto time = SDL_GetTicks();
  auto now = SDL_GetTicks();
  if (now - time > 50) {
    time = now;
    script::Module_Event::emit(_script, "tick");
  }
  script::Module_Event::emit(_script, "update");
  _renderer->begin(camera);
  _renderer->draw(quadMaterial, quad, model);
  _renderer->end();
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
  script::Module_Event::emit(_script, "keyDown",
                             createNumber(_script, e.getScancode()));
  if (e.getScancode() == SDL_SCANCODE_ESCAPE) {
    _mouse->releaseMouse();
  }
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
  _mouse->captureMouse();
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

void GameApplication::onResize(runtime::Event_Resize &e) {
  _renderer->setViewport({0, 0, e.getSize()});
}