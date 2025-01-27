#include "runtime/ClientProxy.hpp"
#include "core/Exception.hpp"
#include "core/Registry.hpp"
#include "core/Singleton.hpp"
#include "render/opengl/OpenGLRenderer.hpp"
#include "runtime/Configuration.hpp"
#include "runtime/OpenGLWindow.hpp"
#include "runtime/VulkanWindow.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <thread>

using namespace firefly;
using namespace firefly::runtime;
using namespace std::chrono;
ClientProxy::ClientProxy() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw core::Exception(fmt::format("Failed to init SDL:{}", SDL_GetError()));
  }
  auto conf = core::Singleton<Configuration>::instance()->getConfig();
  auto title = conf.project.window.title;
  auto width = conf.project.window.width;
  auto height = conf.project.window.height;
  auto x = conf.project.window.x;
  auto y = conf.project.window.y;

  auto renderer = conf.project.device.renderer;

  if (renderer.type.starts_with("OpenGL")) {
    _renderer = new render::OpenGLRenderer();
  }

  if (_renderer) {
    core::Registry::store(typeid(render::Renderer).name(), _renderer);
  }

  if (renderer.type.starts_with("OpenGL")) {
    _window = new OpenGLWindow(title, width, height, x, y);
  } else if (renderer.type.starts_with("Vulkan")) {
    _window = new VulkanWindow(title, width, height, x, y);
  } else {
    _window = new Window(title, width, height, x, y);
  }
  if (_renderer) {
    _renderer->initialize(_window);
  }
}

ClientProxy::~ClientProxy() {
  core::Registry::store(typeid(render::Renderer).name(), nullptr);
  _window = nullptr;
  SDL_Quit();
}

int ClientProxy::run() {
  bool isRunning = true;
  SDL_Event event;
  while (isRunning) {
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        isRunning = false;
      }
    } else {
      std::this_thread::sleep_for(4ms);
    }
    if (_renderer) {
      _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});
      _renderer->present();
    }
  }
  return 0;
}