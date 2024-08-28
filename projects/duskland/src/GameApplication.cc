#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/BufferUsage.hpp"
#include "gl/DataType.hpp"
#include "gl/Program.hpp"
#include "gl/Shader.hpp"
#include "gl/ShaderType.hpp"
#include "gl/Texture.hpp"
#include "gl/VertexArray.hpp"
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
#include <SDL_image.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <lua.hpp>
#include <stdexcept>

using namespace firefly;
using namespace duskland;
core::AutoPtr<gl::Buffer> vbo;
core::AutoPtr<gl::Buffer> cbo;
core::AutoPtr<gl::Buffer> ebo;
core::AutoPtr<gl::VertexArray> vao;
core::AutoPtr<gl::Program> program;
core::AutoPtr<gl::Texture> tex;
float position[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
uint32_t indices[] = {0, 1, 2};
float color[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
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

  vbo = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
  vbo->setData(sizeof(position), position);
  ebo = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
  ebo->setData(sizeof(indices), indices);
  cbo = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
  cbo->setData(sizeof(color), color);
  vao = new gl::VertexArray();
  gl::VertexArray::bind(vao);
  gl::Buffer::bind(gl::BUFFER_TARGET::ELEMENT_ARRAY, ebo);

  gl::Buffer::bind(gl::BUFFER_TARGET::ARRAY, vbo);
  vao->setAttribute(0, gl::DATA_TYPE::FLOAT, 3, false, 3 * sizeof(float), 0);
  vao->enableAttribute(0);
  gl::Buffer::bind(gl::BUFFER_TARGET::ARRAY, cbo);
  vao->setAttribute(1, gl::DATA_TYPE::FLOAT, 3, false, 3 * sizeof(float), 0);
  vao->enableAttribute(1);

  auto vss = _media->load("shader::sprite_2d::vertex.glsl")->read();
  auto fss = _media->load("shader::sprite_2d::fragment.glsl")->read();

  core::AutoPtr vs = new gl::Shader(gl::SHADER_TYPE::VERTEX_SHADER);
  vs->setShaderSource(
      std::string((const char *)vss->getData(), vss->getSize()));

  core::AutoPtr fs = new gl::Shader(gl::SHADER_TYPE::FRAGMENT_SHADER);
  fs->setShaderSource(
      std::string((const char *)fss->getData(), fss->getSize()));

  if (!vs->compile()) {
    throw std::runtime_error(vs->getInfoLog());
  }

  if (!fs->compile()) {
    throw std::runtime_error(fs->getInfoLog());
  }

  program = new gl::Program();
  program->attach(vs);
  program->attach(fs);
  if (!program->link()) {
    throw std::runtime_error(program->getInfoLog());
  }
  glClearColor(0.2, 0.3, 0.3, 1.0f);
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  program->use();
  gl::VertexArray::bind(vao);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
  getWindow()->present();
}

void GameApplication::onUnInitialize() {
  _script->dispose();
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