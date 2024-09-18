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
#include "video/Material.hpp"
#include "video/ModelSet.hpp"
#include "video/PerspectiveCamera.hpp"
#include "video/Renderer.hpp"
#include <SDL_image.h>
#include <SDL_mouse.h>
#include <SDL_scancode.h>
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

#define SCR_WIDTH 1024
#define SCR_HEIGHT 768

using namespace firefly;
using namespace duskland;
float pitch = 0;
float yaw = 90;
float strength = 0.1f;
core::AutoPtr<video::Camera> camera;
core::AutoPtr<video::ModelSet> modelSet;

glm::mat4 model =
    glm::rotate(glm::mat4(1.0), glm::radians(45.0f), glm::vec3(0, 1, 0));
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
  modelSet = video::ModelSet::get("backpack.obj", "model::model.obj");
  auto materials = modelSet->getAllMaterials();
  for (auto &[name, material] : materials) {
    material->enableAttribute(video::Material::HEIGHT_TEX);
    material->enableAttribute(video::Material::SPECULAR_TEX);
  }
  _renderer->getLight()->getAmbientLight()->setStrength(strength);
  camera = new video::PerspectiveCamera(glm::vec3(0, 0, -3.0f),
                                        glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  auto &plight = _renderer->getLight()->getPointLight("self");
  camera->setPosition({0, 0, -5});
  plight->setPosition(camera->getPosition());
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
  float speed = 0.001f;
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
  if (_keyboard->getKeyState(SDL_SCANCODE_LEFT)) {
    model = glm::translate(model, glm::vec3(-speed, 0, 0));
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_RIGHT)) {
    model = glm::translate(model, glm::vec3(speed, 0, 0));
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_UP)) {
    model = glm::translate(model, glm::vec3(0, speed, 0));
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_DOWN)) {
    model = glm::translate(model, glm::vec3(0, -speed, 0));
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_Q)) {
    model = glm::rotate(model, speed, glm::vec3(0, 1, 0));
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_E)) {
    model = glm::rotate(model, -speed, glm::vec3(0, 1, 0));
  }
  if (_keyboard->getKeyState(SDL_SCANCODE_R)) {
    model = glm::mat4(1.0f);
  }
  static int count = 0;
  _renderer->getConstants()->setField("model", model);
  _renderer->begin(camera);
  _renderer->draw(modelSet);
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
  if (e.getScancode() == SDL_SCANCODE_RETURN) {
    for (auto &[name, material] : modelSet->getAllMaterials()) {
      if (material->isWireframe()) {
        material->setIsWireframe(false);
      } else {
        material->setIsWireframe(true);
      }
    }
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
    pitch += -delta.y * 0.05f;
    yaw += delta.x * 0.05f;
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
