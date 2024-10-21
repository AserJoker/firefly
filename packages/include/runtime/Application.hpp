//
// Created by w30029682 on 2024/7/5.
//
#pragma once

#include "BaseApplication.hpp"
#include "Event_Exit.hpp"
#include "Event_SDL.hpp"
#include "runtime/Event_Resize.hpp"
#include <SDL2/SDL.h>

namespace firefly::runtime {
class Application : public BaseApplication {
private:
public:
  Application(int argc, char **argv);

protected:
  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;

  virtual void onEvent(Event_SDL &);

  virtual void onResize(Event_Resize &);

  virtual void onExit(Event_Exit &);
};
} // namespace firefly::runtime