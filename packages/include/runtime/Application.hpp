//
// Created by w30029682 on 2024/7/5.
//
#pragma once

#include "BaseApplication.hpp"
#include "ExitEvent.hpp"
#include "SystemEvent.hpp"
#include "runtime/ResizeEvent.hpp"
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

  virtual void onEvent(SystemEvent &);

  virtual void onResize(ResizeEvent &);

  virtual void onExit(ExitEvent &);
};
} // namespace firefly::runtime