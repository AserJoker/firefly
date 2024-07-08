//
// Created by w30029682 on 2024/7/5.
//
#include "runtime/Application.hpp"
#include "runtime/Event_Quit.hpp"
#include <SDL2/SDL.h>
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
  SDL_Quit();
  BaseApplication::onUnInitialize();
}

void Application::onQuit(Event_Quit &) { exit(); }
