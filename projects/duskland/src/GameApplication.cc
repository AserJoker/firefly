#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "document/Animation.hpp"
#include "document/Camera2D.hpp"
#include "document/Font.hpp"
#include "document/Node.hpp"
#include "document/Scene2D.hpp"
#include "document/Sprite2D.hpp"
#include "document/Text2D.hpp"
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

void GameApplication::initDocument() {
  document::Sprite2D::registerNode();
  document::Scene2D::registerNode();
  document::Window::registerNode();
  document::Camera2D::registerNode();
  document::Font::registerNode();
  document::Text2D::registerNode();
  document::Animation::registerNode();
}

void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _mouse = core::Singleton<input::Mouse>::instance();
  _keyboard = core::Singleton<input::Keyboard>::instance();
  initDocument();
  initEvent();
  initLocale();
  initScript();
  _mod->loadAll(cwd().append("mods").string());
  _locale->reload();
  _document = document::Node::load("duskland::index.xml");
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  _document->onMainLoop();
  if (!SDL_GL_GetCurrentWindow()) {
    exit();
  }
}

void GameApplication::onUnInitialize() {
  runtime::Application::onUnInitialize();
}

void GameApplication::onKeyDown(input::KeyDownEvent &e) {}

void GameApplication::onKeyUp(input::KeyUpEvent &e) {}

void GameApplication::onMouseMotion(input::MouseMotionEvent &e) {}

void GameApplication::onMouseDown(input::MouseDownEvent &e) {
  auto animation =
      document::Node::select("animation").cast<document::Animation>();
  animation->createChannel("position.x", 0, 50, 100, 200);
  animation->createChannel("position.x", 50, 100, 200, 100);
  animation->start();
}

void GameApplication::onMouseWheel(input::MouseWheelEvent &e) {}

void GameApplication::onClick(input::ClickEvent &e) {}

void GameApplication::onResize(runtime::ResizeEvent &e) {}

void GameApplication::onExit(runtime::ExitEvent &e) { Application::onExit(e); }