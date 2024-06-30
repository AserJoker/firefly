#include "runtime/Application.hpp"
#include "core/Coroutine.hpp"
#include "core/EventLoop.hpp"
#include "core/Singleton.hpp"
using namespace firefly;
using namespace firefly::runtime;
void Application::CoSchedule() {
  auto loop = core::Singleton<core::EventLoop>::instance();
  if (!core::CoContext::ready()) {
    core::CoContext::yield();
    loop->start(CoSchedule);
  }
}

void Application::Initialize() {
  auto &loop = core::Singleton<core::EventLoop>::instance();
  if (loop == nullptr) {
    loop = new core::EventLoop();
  }
}

Application::Application(int argc, char *argv[]) : _args(argc), _nExitCode(0) {
  for (int index = 0; index < argc; index++) {
    _args[index] = argv[index];
  }
  Initialize();
  auto &loop = core::Singleton<core::EventLoop>::instance();
  loop->start(CoSchedule);
}

Application::~Application() { core::CoContext::dispose(); }

int Application::run() {
  auto loop = core::Singleton<core::EventLoop>::instance();
  loop->run();
  return _nExitCode;
}

void Application::exit(int nExitCode) {
  auto loop = core::Singleton<core::EventLoop>::instance();
  loop->stop();
  _nExitCode = nExitCode;
}