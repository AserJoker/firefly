#pragma once

#include "CmdLine.hpp"
#include "ConfigProvider.hpp"
#include "EventBus.hpp"
#include "Logger.hpp"
#include "core/EventLoop.hpp"
#include "core/Injector.hpp"
#include "core/Object.hpp"
#include "injector.hpp"
#include <filesystem>
#include <string>
#include <vector>

namespace firefly::runtime {
class BaseApplication : public core::Object {
private:
  std::vector<std::string> _args;
  int _exitcode;
  bool _running;

private:
  void showHelp();

protected:
  core::Injector<core::EventLoop, INJECTOR_EVENTLOOP> _loop;
  core::Injector<Logger, INJECTOR_LOGGER> _logger;
  core::Injector<CmdLine, INJECTOR_CMDLINE> _cmdline;
  core::Injector<ConfigProvider, INJECTOR_CONFIGPROVIDER> _config;
  core::Injector<EventBus, INJECTOR_EVENTBUS> _eventbus;

protected:
  virtual void onInitialize();

  virtual void onMainLoop();

  virtual void onUnInitialize();

public:
  BaseApplication(int argc, char *argv[]);

  ~BaseApplication() override;

  int run();

  void exit(int nExitCode = 0);

  std::filesystem::path cwd();
};
} // namespace firefly::runtime