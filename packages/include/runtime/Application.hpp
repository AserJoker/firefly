//
// Created by w30029682 on 2024/7/5.
//
#pragma once

#include "BaseApplication.hpp"
#include "Event_SDL.hpp"

namespace firefly::runtime {
class Application : public BaseApplication {

public:
  Application(int argc, char **argv);

protected:
  void onInitialize() override;

  virtual void onMainLoop();

  void onUnInitialize() override;

  virtual void onEvent(Event_SDL &);
};
} // namespace firefly::runtime