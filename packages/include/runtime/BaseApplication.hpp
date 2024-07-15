#pragma once

#include "CmdLine.hpp"
#include "Component.hpp"
#include "EventBus.hpp"
#include "Logger.hpp"
#include "ResourceProvider.hpp"
#include "core/EventLoop.hpp"
#include "core/Injector.hpp"
#include "core/Object.hpp"
#include "db/Database.hpp"
#include <filesystem>
#include <string>
#include <vector>

namespace firefly::runtime {
class BaseApplication : public core::Object {
private:
  std::vector<std::string> _args;
  int _exitcode;

private:
  void showHelp();

  void execCoTask();

protected:
  core::Injector<core::EventLoop, INJECTOR_EVENTLOOP> _loop;
  core::Injector<Logger, INJECTOR_LOGGER> _logger;
  core::Injector<CmdLine, INJECTOR_CMDLINE> _cmdline;
  core::Injector<EventBus, INJECTOR_EVENTBUS> _eventbus;
  core::Injector<ResourceProvider, INJECTOR_RESOURCEPROVIDER> _resources;
  core::Injector<db::Database, INJECTOR_DATABASE> _database;

protected:
  virtual void onInitialize();

  virtual void onUnInitialize();

public:
  BaseApplication(int argc, char *argv[]);

  ~BaseApplication() override;

  int run();

  void exit(int nExitCode = 0);

  std::filesystem::path cwd();
};
} // namespace firefly::runtime