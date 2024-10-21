#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Rect.hpp"
#include "core/Singleton.hpp"
#include "gl/DrawMode.hpp"
#include "gl/Texture2D.hpp"
#include "input/Event_Click.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_MouseDown.hpp"
#include "input/Event_MouseMotion.hpp"
#include "input/Event_MouseWheel.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "internal/geometry.hpp"
#include "runtime/Application.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>
#include <fmt/format.h>
#include <glm/ext/matrix_transform.hpp>

using namespace firefly;
using namespace duskland;

glm::mat4 projection =
    glm::scale(glm::mat4(1.0f), {1.0f / 512.0f, 1.0f / 384.0f, 0.0f});

glm::mat4 view = glm::scale(glm::mat4(1.0f), {1.0f, -1.0f, 1.0f}) *
                 glm::translate(glm::mat4(1.0f), {-512.0f, -384.0f, 0.0f});

glm::mat4 model = glm::mat4(1.0f);

core::AutoPtr<video::Geometry> geometry;

core::AutoPtr<video::Material> material;

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
  _window = SDL_CreateWindow("duskland", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 1024, 768,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
                                 SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
  _renderer = new video::Renderer(_window);
  SDL_ShowWindow(_window);
  glClearColor(0.2f, 0.3f, 0.2f, 1.0f);

  _renderer->setShader("2d");

  core::AutoPtr attrPosition = new video::Attribute(internal::rectPosition, 3);
  core::AutoPtr attrTextureCoord =
      new video::Attribute(internal::rectTextureCoord, 2);

  core::AutoPtr attrIndices = new video::AttributeIndex(internal::rectIndices);

  geometry = new video::Geometry();

  geometry->setAttribute(0, attrPosition);
  geometry->setAttribute(1, attrTextureCoord);
  geometry->setAttributeIndex(attrIndices);

  material = new video::Material();
  material->setShader("basic");
  material->setTexture("diffuse", "001-Fighter01.png");
  material->setDepthTest(true);
  material->setIsTransparent(true);

  _renderer->setUniform("projection", projection);

  auto tex =
      gl::Texture2D::get("001-Fighter01.png", "texture::001-Fighter01.png");

  model = glm::scale(glm::mat4(1.0f), {tex->getSize().width * 1.0f,
                                       tex->getSize().height * 1.0f, 1.0f});
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  _renderer->setUniform("view", view);
  _renderer->draw(material, geometry, model);
  _renderer->present();
  SDL_GL_SwapWindow(_window);
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
  _renderer->setViewport({0, 0, e.getSize()});
}

void GameApplication::onExit(runtime::Event_Exit &e) { Application::onExit(e); }