#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "document/Camera2D.hpp"
#include "document/Node.hpp"
#include "document/Renderable.hpp"
#include "document/Scene2D.hpp"
#include "document/Window.hpp"
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
#include "video/Texture.hpp"
#include <SDL_video.h>
#include <fmt/format.h>
#include <glm/ext/matrix_transform.hpp>

using namespace firefly;
using namespace duskland;

core::AutoPtr<document::Node> root = new document::Node();

core::AutoPtr<document::Camera2D> camera;

class Demo : public document::Renderable {
private:
  glm::mat4 model = glm::mat4(1.0f);

  core::AutoPtr<video::Geometry> geometry;

  core::AutoPtr<video::Material> material;

protected:
  const core::AutoPtr<video::Geometry> &getGeometry() const override {
    return geometry;
  }

  const core::AutoPtr<video::Material> &getMaterial() const override {
    return material;
  }

  const glm::mat4 &getMatrix() const override { return model; }

public:
  Demo() {
    core::AutoPtr attrPosition =
        new video::Attribute(internal::rectPosition, 3);
    core::AutoPtr attrTextureCoord =
        new video::Attribute(internal::rectTextureCoord, 2);

    core::AutoPtr attrIndices =
        new video::AttributeIndex(internal::rectIndices);

    geometry = new video::Geometry();

    geometry->setAttribute(0, attrPosition);
    geometry->setAttribute(1, attrTextureCoord);
    geometry->setAttributeIndex(attrIndices);

    material = new video::Material();
    material->setShader("basic");
    material->setTexture("diffuse", new video::Texture("001-Fighter01.png"));
    material->setDepthTest(true);
    material->setIsTransparent(true);

    auto tex = material->getTexture("diffuse");
    model = glm::translate(glm::mat4(1.0f), {0.0f, 0, 0}) *
            glm::scale(glm::mat4(1.0f),
                       {tex->getTexture()->getSize().width * 1.0f,
                        tex->getTexture()->getSize().height * 1.0f, 1.0f});
    tex->setMatrix(glm::scale(glm::mat4(1.0f), {1, 1, 1.0}) *
                   glm::translate(glm::mat4(1.0f), {0, 0, 0}));
  }
};

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

  core::AutoPtr<document::Node> window = new document::Window();
  root->appendChild(window);

  core::AutoPtr<document::Node> scene = new document::Scene2D();
  window->appendChild(scene);
  camera = new document::Camera2D();
  scene->appendChild(camera);
  scene->appendChild(new Demo());
  glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  root->onTick();
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

void GameApplication::onResize(runtime::Event_Resize &e) {}

void GameApplication::onExit(runtime::Event_Exit &e) { Application::onExit(e); }