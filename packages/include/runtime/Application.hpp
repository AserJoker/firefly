//
// Created by w30029682 on 2024/7/5.
//
#pragma once

#include "BaseApplication.hpp"
#include "Event_SDL.hpp"
#include <SDL2/SDL.h>

namespace firefly::runtime {
class Application : public BaseApplication {
private:
  SDL_Window *_window;
  SDL_GLContext _ctx;

public:
  Application(int argc, char **argv);

protected:
  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;

  void showWindow(bool show);

  virtual void onEvent(Event_SDL &);
};
} // namespace firefly::runtime