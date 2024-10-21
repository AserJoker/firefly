//
// Created by w30029682 on 2024/7/5.
//
#include "runtime/Application.hpp"
#include "core/AutoPtr.hpp"
#include "exception/SDLException.hpp"
#include "runtime/BaseApplication.hpp"
#include "runtime/Event_Exit.hpp"
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
  _eventbus->on(this, &Application::onEvent);
  _eventbus->on(this, &Application::onResize);
  _eventbus->on(this, &Application::onExit);
}

void Application::onMainLoop() {
  BaseApplication::onMainLoop();
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    _eventbus->emit<Event_SDL>(event);
  }
}

void Application::onUnInitialize() {
  TTF_Quit();
  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
  BaseApplication::onUnInitialize();
}

void Application::onEvent(Event_SDL &event) {
  if (event.getEvent().type == SDL_QUIT) {
    _eventbus->emit<Event_Exit>();
  }
  if (event.getEvent().type == SDL_WINDOWEVENT) {
    if (event.getEvent().window.event == SDL_WINDOWEVENT_RESIZED) {
      uint32_t width = event.getEvent().window.data1;
      uint32_t height = event.getEvent().window.data2;
      _eventbus->emit<Event_Resize>(core::Size<>(width, height));
    }
  }
}

void Application::onExit(Event_Exit &) { exit(); }

void Application::onResize(Event_Resize &) {}
