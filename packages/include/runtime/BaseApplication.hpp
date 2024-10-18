#pragma once

#include "CmdLine.hpp"
#include "Component.hpp"
#include "EventBus.hpp"
#include "Locale.hpp"
#include "Logger.hpp"
#include "Media.hpp"
#include "ModLoader.hpp"
#include "core/Injector.hpp"
#include "core/Object.hpp"
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
  core::Injector<Logger, INJECTOR_LOGGER> _logger;
  core::Injector<CmdLine, INJECTOR_CMDLINE> _cmdline;
  core::Injector<EventBus, INJECTOR_EVENTBUS> _eventbus;
  core::Injector<Media, INJECTOR_MEDIA> _media;
  core::Injector<Locale, INJECTOR_LOCALE> _locale;
  core::Injector<ModLoader, INJECTOR_MOD> _mod;

protected:
  virtual void onInitialize();

  virtual void onUnInitialize();

  virtual void onMainLoop();

public:
  BaseApplication(int argc, char *argv[]);

  ~BaseApplication() override;

  int run();

  void exit(int nExitCode = 0);

  std::filesystem::path cwd();
};
} // namespace firefly::runtime