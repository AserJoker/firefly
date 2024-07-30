#pragma once

#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Event_MouseWheel.hpp"
#include "runtime/Application.hpp"

namespace duskland {
using namespace firefly;

class GameApplication : public runtime::Application {

protected:
  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;

  virtual void initScript();
  virtual void initLocale();
  virtual void initEvent();

  void onMouse(input::Event_Mouse &e);
  void onMouseButtonDown(input::Event_MouseButtonDown &e);
  void onKeydown(input::Event_KeyDown &e);
  void onMouseWheel(input::Event_MouseWheel &e);

public:
  GameApplication(int argc, char *argv[]);
};
} // namespace duskland