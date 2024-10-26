#pragma once

#include "core/AutoPtr.hpp"
#include "document/Node.hpp"
#include "input/ClickEvent.hpp"
#include "input/KeyDownEvent.hpp"
#include "input/KeyUpEvent.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "input/MouseDownEvent.hpp"
#include "input/MouseMotionEvent.hpp"
#include "input/MouseWheelEvent.hpp"
#include "runtime/Application.hpp"
#include "runtime/ResizeEvent.hpp"

namespace duskland {
using namespace firefly;

class GameApplication : public runtime::Application {
private:
  core::AutoPtr<input::Mouse> _mouse;
  core::AutoPtr<input::Keyboard> _keyboard;
  core::AutoPtr<document::Node> _document;

protected:
  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;

  void initScript();
  void initLocale();
  void initEvent();
  void initDocument();

  void onMouseMotion(input::MouseMotionEvent &e);
  void onMouseDown(input::MouseDownEvent &e);
  void onMouseWheel(input::MouseWheelEvent &e);
  void onClick(input::ClickEvent &e);
  void onKeyDown(input::KeyDownEvent &e);
  void onKeyUp(input::KeyUpEvent &e);

  void onResize(runtime::ResizeEvent &e) override;

  void onExit(runtime::ExitEvent &) override;

public:
  GameApplication(int argc, char *argv[]);
};
} // namespace duskland