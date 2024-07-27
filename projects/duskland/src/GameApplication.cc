#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Event_MouseWheel.hpp"
#include "runtime/Application.hpp"
#include "script/LuaValue.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
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
void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _resources->setCurrentWorkspaceDirectory(cwd().append("media").string());
  _eventbus->on(this, &GameApplication::onMouse);
  _eventbus->on(this, &GameApplication::onKeydown);
  _eventbus->on(this, &GameApplication::onMouseButtonDown);
  _eventbus->on(this, &GameApplication::onMouseWheel);
  _window = new runtime::Window("duskland", 1024, 768);
  _renderer = new video::Renderer();
  _renderer->enableDepthTest();
  _script->eval(
      "package.path = package.path..';.//media/lua/?.lua;./media/lua/?.so'");
  auto func = script::LuaValue::create(
      _script->getLuaContext(),
      std::function([](lua_State *state,
                       const std::vector<core::AutoPtr<script::LuaValue>> &args)
                        -> std::vector<core::AutoPtr<script::LuaValue>> {
        return {script::LuaValue::create(state, args[0]->toInteger() +
                                                    args[1]->toInteger())};
      }));
  auto global = script::LuaValue::getGlobal(_script->getLuaContext());
  global->setField("add", func);
  _script->eval("require 'init'");
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  _window->present();
}

void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }

void GameApplication::onMouse(input::Event_Mouse &e) {}

void GameApplication::onMouseButtonDown(input::Event_MouseButtonDown &e) {}

void GameApplication::onKeydown(input::Event_KeyDown &e) {}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {}