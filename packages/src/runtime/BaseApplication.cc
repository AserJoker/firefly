#include "runtime/BaseApplication.hpp"
#include "core/Coroutine.hpp"
#include <filesystem>
#include <iostream>

using namespace firefly;
using namespace firefly::runtime;

BaseApplication::BaseApplication(int argc, char *argv[])
    : _args(argc), _exitcode(0) {
  for (int index = 0; index < argc; index++) {
    _args[index] = argv[index];
  }
  _cmdline->define("help", _cmdline->placeholder, "h",
                   "Display this information.");
  _loop->start([this]() -> void { this->execCoTask(); });
}

BaseApplication::~BaseApplication() { core::CoContext::dispose(); }

int BaseApplication::run() {
  auto parsed = _cmdline->parse(_args);
  if (parsed.contains("help")) {
    showHelp();
    return 0;
  }
  _media->addCurrentWorkspaceDirectory(cwd().string());
  onInitialize();
  while (!_loop->ready()) {
    _loop->nextTick();
  }
  onUnInitialize();
  return _exitcode;
}

void BaseApplication::exit(int exitcode) {
  _exitcode = exitcode;
  _loop->stop();
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

void BaseApplication::onInitialize() {}

void BaseApplication::onUnInitialize() {}

void BaseApplication::execCoTask() {
  core::CoContext::yield();
  _loop->start([this]() -> void { this->execCoTask(); });
}