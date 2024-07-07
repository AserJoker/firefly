//
// Created by w30029682 on 2024/7/5.
//
#include "runtime/Application.hpp"
#include "runtime/IWindow.hpp"
#include "runtime/Window_OpenGL.hpp"
#include <SDL2/SDL.h>
#include <SDL_error.h>
#include <stdexcept>

using namespace firefly;
using namespace firefly::runtime;

Application::Application(int argc, char **argv) : BaseApplication(argc, argv) {}

void Application::onInitialize() {
  BaseApplication::onInitialize();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

void Application::onMainLoop() { BaseApplication::onMainLoop(); }

void Application::onUnInitialize() {
  SDL_Quit();
  BaseApplication::onUnInitialize();
}

core::AutoPtr<IWindow> Application::createWindow(const std::string &title,
                                                 int width, int height,
                                                 GRAPHIC_API api) {
  switch (api) {
  case GRAPHIC_API::NONE:
    break;
  case GRAPHIC_API::OPENGL:
    return new Window_OpenGL(title, width, height);
  case GRAPHIC_API::VULKAN:
    break;
  case GRAPHIC_API::DIRECTX12:
    break;
  case GRAPHIC_API::DIRECTX11:
    break;
  }
  return nullptr;
}
void Application::processEvent() {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit();
    }
  }
}