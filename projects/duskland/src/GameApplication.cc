#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "document/Camera2D.hpp"
#include "document/Node.hpp"
#include "document/Scene2D.hpp"
#include "document/Sprite2D.hpp"
#include "document/Window.hpp"
#include "input/ClickEvent.hpp"
#include "input/KeyDownEvent.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "input/MouseDownEvent.hpp"
#include "input/MouseMotionEvent.hpp"
#include "input/MouseWheelEvent.hpp"
#include "runtime/Application.hpp"
#include <SDL_video.h>
#include <fmt/format.h>
#include <glm/ext/matrix_transform.hpp>

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
  _document = new document::Node();

  core::AutoPtr<document::Node> window = new document::Window();
  _document->appendChild(window);
  core::AutoPtr<document::Node> scene = new document::Scene2D();
  window->appendChild(scene);
  core::AutoPtr camera = new document::Camera2D();
  scene->appendChild(camera);
  core::AutoPtr sprite = new document::Sprite2D();
  core::AutoPtr sprite2 = new document::Sprite2D();
  sprite->setTexture("001-Fighter01.png");
  sprite2->setTexture("001-Fighter01.png");

  sprite->setDestinationRect({0, 0, 32, 48});
  sprite->setSourceRect({0, 0, 32, 48});

  sprite2->setDestinationRect({5 * 32, 0, 32, 48});
  sprite2->setSourceRect({0, 0, 32, 48});

  core::AutoPtr grass = new document::Sprite2D();
  grass->setTexture("001-Grassland01.png");

  grass->setDestinationPosition({0, 16});

  scene->appendChild(sprite);
  scene->appendChild(grass);
  scene->appendChild(sprite2);
  sprite->setZIndex(1);
  sprite2->setZIndex(-1);
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  _document->onMainLoop();
}

void GameApplication::onUnInitialize() {
  runtime::Application::onUnInitialize();
}

void GameApplication::onKeyDown(input::KeyDownEvent &e) {}

void GameApplication::onKeyUp(input::KeyUpEvent &e) {}

void GameApplication::onMouseMotion(input::MouseMotionEvent &e) {}

void GameApplication::onMouseDown(input::MouseDownEvent &e) {}

void GameApplication::onMouseWheel(input::MouseWheelEvent &e) {}

void GameApplication::onClick(input::ClickEvent &e) {}

void GameApplication::onResize(runtime::ResizeEvent &e) {}

void GameApplication::onExit(runtime::ExitEvent &e) { Application::onExit(e); }