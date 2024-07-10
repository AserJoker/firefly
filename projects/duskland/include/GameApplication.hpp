#pragma once

#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "runtime/Application.hpp"
#include "runtime/Window.hpp"
#include "video/Renderer.hpp"

namespace duskland {
using namespace firefly;

class GameApplication : public runtime::Application {

private:
  core::AutoPtr<runtime::Window> _window;
  core::AutoPtr<video::Renderer> _renderer;

protected:
  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;

public:
  GameApplication(int argc, char *argv[]);

  void onMouse(input::Event_Mouse &e);
  void onMouseButtonDown(input::Event_MouseButtonDown &e);
  void onKeydown(input::Event_KeyDown &e);
};
} // namespace duskland