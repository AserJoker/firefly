//
// Created by w30029682 on 2024/7/5.
//
#pragma once

#include "BaseApplication.hpp"
#include "Event_SDL.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Window.hpp"

namespace firefly::runtime {
class Application : public BaseApplication {
private:
  core::AutoPtr<Window> _window;

public:
  Application(int argc, char **argv);
  core::AutoPtr<Window> getWindow();

protected:
  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;

  virtual void onEvent(Event_SDL &);
};
} // namespace firefly::runtime