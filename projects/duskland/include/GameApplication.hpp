#pragma once
#include "firefly.hpp"
namespace duskland {
using namespace firefly;
class GameApplication : public runtime::Application {
private:
  core::AutoPtr<runtime::IWindow> _window;
  core::AutoPtr<video::IRenderer> _renderer;

protected:
  void onInitialize() override;
  void onMainLoop() override;
  void onUnInitialize() override;

public:
  GameApplication(int argc, char *argv[]);
};
} // namespace duskland