#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "core/Value.hpp"
#include "input/Event_Click.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_MouseDown.hpp"
#include "input/Event_MouseMotion.hpp"
#include "input/Event_MouseWheel.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "runtime/Application.hpp"
#include "video/Renderer.hpp"
#include <SDL_image.h>
#include <SDL_mouse.h>
#include <SDL_timer.h>
#include <fmt/format.h>
#include <glad/glad.h>
#include <iostream>
#include <string>

using namespace firefly;
using namespace duskland;

GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv){};

void GameApplication::initScript() {}

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
  glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
  _renderer = new video::Renderer();
  using namespace core;
  Value v = 123;
  v.get<Integer_t>() = 234;
  exit();
  showWindow(true);
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  _renderer->present();
}

void GameApplication::onUnInitialize() {
  runtime::Application::onUnInitialize();
}

void GameApplication::onKeyDown(input::Event_KeyDown &e) {}

void GameApplication::onKeyUp(input::Event_KeyUp &e) {}

void GameApplication::onMouseMotion(input::Event_MouseMotion &e) {}

void GameApplication::onMouseDown(input::Event_MouseDown &e) {}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {}

void GameApplication::onClick(input::Event_Click &e) {}

void GameApplication::onResize(runtime::Event_Resize &e) {
  _renderer->setViewport({core::Point<>(0, 0), e.getSize()});
}