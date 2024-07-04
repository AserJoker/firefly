#include "runtime/OpenGLApplication.hpp"
#include "core/EventLoop.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "runtime/CmdLine.hpp"
#include "runtime/Logger.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdexcept>

using namespace firefly::runtime;
#define SDL_ASSERT(expr)                                                       \
  if (expr) {                                                                  \
    throw std::runtime_error(SDL_GetError());                                  \
  }

void OpenGLApplication::onInitialize() {
    SDL_ASSERT(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    initOpenGL();
    _mainWindow =
            SDL_CreateWindow("firefly", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
    SDL_ASSERT(!_mainWindow)
    _glContext = SDL_GL_CreateContext(_mainWindow);
    SDL_ASSERT(!_glContext)
    if (gladLoadGLLoader(SDL_GL_GetProcAddress) < 0) {
        throw std::runtime_error("failed to init opengl");
    }
    auto loop = core::Singleton<core::EventLoop>::instance();
    loop->start([this]() -> void { this->onMainLoop(); });
}

void OpenGLApplication::onMainLoop() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit();
            return;
        }
    }
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SDL_GL_SwapWindow(_mainWindow);
    auto loop = core::Singleton<core::EventLoop>::instance();
    loop->start([this]() -> void { this->onMainLoop(); });
}

OpenGLApplication::OpenGLApplication(int argc, char *argv[])
        : Application(argc, argv), _mainWindow(nullptr), _glContext(nullptr), _config({.engine="v4.2 core"}) {
    auto &cmdline = core::Singleton<CmdLine>::instance();
    cmdline->define("opengl", _config.engine, "",
                    "OpenGL Version for render,default is 'v4.2 core',enum values is 'v4.2 core','v4.5 core'");
}

OpenGLApplication::~OpenGLApplication() { SDL_Quit(); }

void OpenGLApplication::initOpenGL() {
    int major = 4;
    int minor = 2;
    int profile = SDL_GL_CONTEXT_PROFILE_CORE;
    if (_config.engine == "v4.2 core") {
        major = 4;
        minor = 2;
        profile = SDL_GL_CONTEXT_PROFILE_CORE;
    } else if (_config.engine == "v4.5 core") {
        major = 4;
        minor = 5;
        profile = SDL_GL_CONTEXT_PROFILE_CORE;
    } else {
        auto logger = core::Singleton<Logger>::instance();
        logger->warn("Unsupported OpenGL version '{}',use default v4.2 core", _config.engine);
    }
#ifdef __APPLE__
    profile |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#endif
    SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major) < 0)
    SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor) < 0)
    SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile) < 0)
}
