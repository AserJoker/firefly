#include "runtime/BaseApplication.hpp"
#include "core/Coroutine.hpp"
#include <filesystem>
#include <iostream>

using namespace firefly;
using namespace firefly::runtime;

BaseApplication::BaseApplication(int argc, char *argv[])
    : _args(), _exitcode(0) {
  _args.resize(argc);
  for (int index = 0; index < argc; index++) {
    _args[index] = argv[index];
  }
  _cmdline->define("help", _cmdline->placeholder, "h",
                   "Display this information.");
}

BaseApplication::~BaseApplication() {}

int BaseApplication::run() {
  _running = true;
  auto parsed = _cmdline->parse(_args);
  if (parsed.contains("help")) {
    showHelp();
    return 0;
  }
  _media->addCurrentWorkspaceDirectory(cwd().string());
  onInitialize();
  while (_running) {
    onMainLoop();
  }
  onUnInitialize();
  return _exitcode;
}

void BaseApplication::exit(int exitcode) {
  _running = false;
  _exitcode = exitcode;
}

std::filesystem::path BaseApplication::cwd() {
  std::filesystem::path dir = _args[0];
  return dir.parent_path();
}

void BaseApplication::showHelp() {
  std::cout << "Usage: firefly [options]" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << _cmdline->help() << std::endl;
}

void BaseApplication::onInitialize() { core::Coroutine::init(); }

void BaseApplication::onUnInitialize() {}

void BaseApplication::onMainLoop() { core::Coroutine::yield(); }