#include "runtime/OpenGLApplication.hpp"
#include "core/EventLoop.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include <SDL2/SDL.h>
#include <stdexcept>
using namespace firefly::runtime;
#define SDL_ASSERT(expr)                                                       \
  if (expr) {                                                                  \
    throw std::runtime_error(SDL_GetError());                                  \
  }
void OpenGLApplication::onInitialize() {
  SDL_ASSERT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) < 0)
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2) < 0)
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                 SDL_GL_CONTEXT_PROFILE_CORE) < 0)
  _mainWindow =
      SDL_CreateWindow("firefly", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
  SDL_ASSERT(!_mainWindow)
  _glContext = SDL_GL_CreateContext(_mainWindow);
  SDL_ASSERT(!_glContext)
  auto loop = core::Singleton<core::EventLoop>::instance();
  loop->start(&OpenGLApplication::onMainLoop);
}
void OpenGLApplication::onMainLoop() {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      auto app = core::Singleton<Application>::instance();
      app->exit();
      return;
    }
  }
  auto loop = core::Singleton<core::EventLoop>::instance();
  loop->start(onMainLoop);
}
OpenGLApplication::OpenGLApplication(int argc, char *argv[])
    : Application(argc, argv) {}
OpenGLApplication::~OpenGLApplication() { SDL_Quit(); }