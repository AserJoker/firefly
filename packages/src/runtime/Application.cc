//
// Created by w30029682 on 2024/7/5.
//
#include "runtime/Application.hpp"
#include "runtime/Event_Quit.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdexcept>

using namespace firefly;
using namespace firefly::runtime;

Application::Application(int argc, char **argv) : BaseApplication(argc, argv) {
  _loop->start([this]() -> void { this->onMainLoop(); });
}

void Application::onInitialize() {
  BaseApplication::onInitialize();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error(SDL_GetError());
  }
  if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP |
                IMG_INIT_JXL | IMG_INIT_AVIF)) {
    throw std::runtime_error(SDL_GetError());
  }
  if (!Mix_Init(MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_MP3 |
                MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK)) {
    throw std::runtime_error(SDL_GetError());
  }
  _eventbus->on(this, &Application::onQuit);
}

void Application::onMainLoop() {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      _eventbus->emit<Event_Quit>();
      break;
    }
  }
  _loop->start([this]() -> void { this->onMainLoop(); });
}

void Application::onUnInitialize() {
  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
  BaseApplication::onUnInitialize();
}

void Application::onQuit(Event_Quit &) { exit(); }
