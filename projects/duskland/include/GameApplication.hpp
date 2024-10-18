#pragma once

#include "core/AutoPtr.hpp"
#include "input/Event_Click.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_KeyUp.hpp"
#include "input/Event_MouseDown.hpp"
#include "input/Event_MouseMotion.hpp"
#include "input/Event_MouseWheel.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "runtime/Application.hpp"
#include "runtime/Event_Resize.hpp"
#include "video/Renderer.hpp"

namespace duskland {
using namespace firefly;

class GameApplication : public runtime::Application {
private:
  core::AutoPtr<input::Mouse> _mouse;
  core::AutoPtr<input::Keyboard> _keyboard;
  core::AutoPtr<video::Renderer> _renderer;

protected:
  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;

  virtual void initScript();
  virtual void initLocale();
  virtual void initEvent();

  void onMouseMotion(input::Event_MouseMotion &e);
  void onMouseDown(input::Event_MouseDown &e);
  void onMouseWheel(input::Event_MouseWheel &e);
  void onClick(input::Event_Click &e);
  void onKeyDown(input::Event_KeyDown &e);
  void onKeyUp(input::Event_KeyUp &e);

  void onResize(runtime::Event_Resize &e);

public:
  GameApplication(int argc, char *argv[]);
};
} // namespace duskland