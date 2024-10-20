//
// Created by w30029682 on 2024/7/5.
//
#include "runtime/Application.hpp"
#include "core/AutoPtr.hpp"
#include "exception/GLADException.hpp"
#include "exception/SDLException.hpp"
#include "runtime/BaseApplication.hpp"
#include "runtime/Event_Resize.hpp"
#include "runtime/Event_SDL.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_video.h>
#include <glad/glad.h>

using namespace firefly;
using namespace firefly::runtime;
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;
  auto logger = core::Singleton<Logger>::instance();
  logger->warn("---------------");
  logger->warn("Debug message ({}): {}", id, message);

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    logger->warn("Source: API");
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    logger->warn("Source: Window System");
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    logger->warn("Source: Shader Compiler");
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    logger->warn("Source: Third Party");
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    logger->warn("Source: Application");
    break;
  case GL_DEBUG_SOURCE_OTHER:
    logger->warn("Source: Other");
    break;
  }

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    logger->warn("Type: Error");
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    logger->warn("Type: Deprecated Behaviour");
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    logger->warn("Type: Undefined Behaviour");
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    logger->warn("Type: Portability");
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    logger->warn("Type: Performance");
    break;
  case GL_DEBUG_TYPE_MARKER:
    logger->warn("Type: Marker");
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    logger->warn("Type: Push Group");
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    logger->warn("Type: Pop Group");
    break;
  case GL_DEBUG_TYPE_OTHER:
    logger->warn("Type: Other");
    break;
  }

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    logger->warn("Severity: high");
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    logger->warn("Severity: medium");
    break;
  case GL_DEBUG_SEVERITY_LOW:
    logger->warn("Severity: low");
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    logger->warn("Severity: notification");
    break;
  }
  logger->warn("---------------");
}
Application::Application(int argc, char **argv) : BaseApplication(argc, argv) {}

void Application::onInitialize() {
  BaseApplication::onInitialize();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw exception::SDLException(SDL_GetError());
  }
  if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP |
                IMG_INIT_JXL | IMG_INIT_AVIF)) {
    throw exception::SDLException(SDL_GetError());
  }
  if (!Mix_Init(MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_MP3 |
                MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK)) {
    throw exception::SDLException(SDL_GetError());
  }
  if (TTF_Init() != 0) {
    throw exception::SDLException(SDL_GetError());
  }
  _media->addCurrentWorkspaceDirectory(cwd().append("media").string());

  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifndef __apple__
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#else
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                 SDL_GL_CONTEXT_PROFILE_CORE |
                                     SDL_GL_CONTEXT_PROFILE_COMPATIBILITY));
#endif
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  _window = SDL_CreateWindow("firefly", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 1024, 768,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
                                 SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
  _ctx = SDL_GL_CreateContext(_window);
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw exception::GLADException("Failed to initialize glad");
  }
  SDL_GL_SetSwapInterval(0);

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }

  _eventbus->on(this, &Application::onEvent);
}
void Application::onMainLoop() {
  BaseApplication::onMainLoop();
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    _eventbus->emit<Event_SDL>(event);
  }
  SDL_GL_SwapWindow(_window);
}

void Application::onUnInitialize() {
  SDL_GL_DeleteContext(_ctx);
  TTF_Quit();
  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
  BaseApplication::onUnInitialize();
}
void Application::showWindow(bool show) {
  if (show) {
    SDL_ShowWindow(_window);
  } else {
    SDL_HideWindow(_window);
  }
}

void Application::onEvent(Event_SDL &event) {
  if (event.getEvent().type == SDL_QUIT) {
    exit();
  }
  if (event.getEvent().type == SDL_WINDOWEVENT) {
    if (event.getEvent().window.event == SDL_WINDOWEVENT_RESIZED) {
      _eventbus->emit<Event_Resize>(
          core::Size<>({(uint32_t)event.getEvent().window.data1,
                        (uint32_t)event.getEvent().window.data2}));
    }
  }
}
