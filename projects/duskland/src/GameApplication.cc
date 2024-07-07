#include "GameApplication.hpp"
#include "runtime/Application.hpp"
using namespace firefly;
using namespace duskland;
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv) {}
void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _window = createWindow("duskland", 1024, 768);
  _renderer = _window->createRenderer();
}
void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  processEvent();
  _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});
  _window->present();
}
void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }