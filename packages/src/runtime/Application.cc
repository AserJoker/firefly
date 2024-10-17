//
// Created by w30029682 on 2024/7/5.
//
#include "runtime/Application.hpp"
#include "core/AutoPtr.hpp"
#include "core/Rect.hpp"
#include "exception/SDLException.hpp"
#include "runtime/BaseApplication.hpp"
#include "runtime/Event_Resize.hpp"
#include "runtime/Event_SDL.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_video.h>
#include <imgui_impl_sdl2.h>

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
  _window = new runtime::Window("firefly", 1024, 768);
  _renderer->initialize({core::Point<>(0, 0), core::Size<>(1024, 768)});
  _renderer->setShader("standard");
  _eventbus->on(this, &Application::onEvent);
}
core::AutoPtr<Window> Application::getWindow() { return _window; }
void Application::onMainLoop() {
  BaseApplication::onMainLoop();
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
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
