#include "runtime/Application.hpp"
#include "core/Coroutine.hpp"
#include "core/EventLoop.hpp"
#include "core/Singleton.hpp"
using namespace firefly;
using namespace firefly::runtime;
void Application::onMainLoop() {
  auto loop = core::Singleton<core::EventLoop>::instance();
  if (!core::CoContext::ready()) {
    core::CoContext::yield();
  }
  auto theApp = core::Singleton<Application>::instance();
  if (theApp->_running) {
    loop->start(onMainLoop);
  }
}

void Application::Initialize() {
  auto &loop = core::Singleton<core::EventLoop>::instance();
  if (loop == nullptr) {
    loop = new core::EventLoop();
  }
}

Application::Application(int argc, char *argv[])
    : _args(argc), _exitcode(0), _running(true) {
  for (int index = 0; index < argc; index++) {
    _args[index] = argv[index];
  }
  Initialize();
  auto &loop = core::Singleton<core::EventLoop>::instance();
  loop->start(onMainLoop);
}

Application::~Application() { core::CoContext::dispose(); }

int Application::run() {
  auto loop = core::Singleton<core::EventLoop>::instance();
  loop->run();
  return _exitcode;
}

void Application::exit(int exitcode) {
  auto loop = core::Singleton<core::EventLoop>::instance();
  _exitcode = exitcode;
  _running = false;
}