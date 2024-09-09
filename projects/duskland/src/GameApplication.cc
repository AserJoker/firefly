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
#include "script/helper/Trait_Properties.hpp"
#include "script/helper/Trait_Resource.hpp"
#include "script/lib/Module_Array.hpp"
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
#include "video/ModelSet.hpp"
#include "video/Renderer.hpp"
#include <fmt/format.h>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <lua.hpp>

using namespace firefly;
using namespace duskland;
float pitch = 0;
float yaw = 90;

core::AutoPtr<video::Camera> camera;
core::AutoPtr<video::ModelSet> modelSet;
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv){};
void GameApplication::initScript() {
  _script->setBridge(new script::LuaBridge());
  _script->eval(
      "package.path = package.path..';./mods/?/lua/init.lua;./lua/?/init.lua'");
  script::Trait_Buffer::initialize(_script);
  script::Trait_Resource::initialize(_script);
  script::Trait_Properties::initialize(_script);
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
  initEvent();
  initLocale();
  initScript();
  _mod->loadAll(cwd().append("mods").string());
  _locale->reload();
  _database->load();
  script::Module_Event::emit(_script, "gameLoaded");
  modelSet = video::ModelSet::get("backpack.obj", "model::backpack.obj");
  camera = new video::Camera(
      glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f),
      glm::vec3(0, 0, -3.0f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
  glClearColor(0.2, 0.3, 0.3, 1.0f);
  glEnable(GL_CULL_FACE);
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
  float speed = 0.001;
  if (_keyboard->getKeyState(SDL_SCANCODE_A)) {
    auto pos = camera->getPosition();
    auto up = camera->getUp();
    auto front = camera->getFront();
    auto right = glm::normalize(glm::cross(front, up));
    pos += right * -speed;
    camera->setPosition(pos);
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_D)) {
    auto pos = camera->getPosition();
    auto up = camera->getUp();
    auto front = camera->getFront();
    auto right = glm::normalize(glm::cross(front, up));
    pos += right * speed;
    camera->setPosition(pos);
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_W)) {
    auto pos = camera->getPosition();
    auto front = camera->getFront();
    pos += front * speed;
    camera->setPosition(pos);
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_S)) {
    auto pos = camera->getPosition();
    auto front = camera->getFront();
    pos -= front * speed;
    camera->setPosition(pos);
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_SPACE)) {
    auto pos = camera->getPosition();
    auto up = camera->getUp();
    pos += up * speed;
    camera->setPosition(pos);
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_LSHIFT)) {
    auto pos = camera->getPosition();
    auto up = camera->getUp();
    pos -= up * speed;
    camera->setPosition(pos);
  }
  // ImGui_ImplOpenGL3_NewFrame();
  // ImGui_ImplSDL2_NewFrame();
  // ImGui::NewFrame();

  // 1. Show the big demo window (Most of the sample code is in
  // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
  // ImGui!).
  // if (show_demo_window)
  //   ImGui::ShowDemoWindow(&show_demo_window);

  // Rendering
  // ImGui::Render();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  _renderer->begin(camera);
  _renderer->draw(modelSet);
  _renderer->end();
  // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
  if (_mouse->isCaptured()) {
    pitch += -delta.y * 0.05;
    yaw += delta.x * 0.05;
    if (pitch > 89.0f) {
      pitch = 89.0f;
    }
    if (pitch < -89.0f) {
      pitch = -89.0f;
    }
    glm::vec3 front;
    front.x = std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    camera->setFront(front);
  }
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
