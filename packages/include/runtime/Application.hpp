#pragma once
#include "core/Coroutine.hpp"
#include "core/Object.hpp"
#include <string>
#include <vector>
namespace firefly::runtime {
class Application : public core::Object {
private:
  std::vector<std::string> _args;
  bool _isRunning;
  int _nExitCode;

public:
  Application(int argc, char *argv[])
      : _args(argc), _isRunning(false), _nExitCode(0) {
    for (int index = 0; index < argc; index++) {
      _args[index] = argv[index];
    }
    core::CoContext::init();
  }
  ~Application() override { core::CoContext::dispose(); }
  int run() {
    _isRunning = true;
    while (_isRunning) {
      if (!core::CoContext::ready()) {
        core::CoContext::yield();
      } else {
        exit();
      }
    }
    return _nExitCode;
  }
  void exit(int nExitCode = 0) {
    _nExitCode = nExitCode;
    _isRunning = false;
  }
};
} // namespace firefly::runtime