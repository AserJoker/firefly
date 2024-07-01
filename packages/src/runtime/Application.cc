#include "runtime/Application.hpp"
#include "core/Coroutine.hpp"
#include "core/EventLoop.hpp"
#include "core/Singleton.hpp"
#include "runtime/EventBus.hpp"
using namespace firefly;
using namespace firefly::runtime;

void Application::Initialize() {
  auto &loop = core::Singleton<core::EventLoop>::instance();
  if (loop == nullptr) {
    loop = new core::EventLoop();
  }
  auto &bus = core::Singleton<EventBus>::instance();
  if (bus == nullptr) {
    bus = new EventBus();
  }
}

Application::Application(int argc, char *argv[])
    : _args(argc), _exitcode(0), _running(true) {
  for (int index = 0; index < argc; index++) {
    _args[index] = argv[index];
  }
  Initialize();
}

Application::~Application() { core::CoContext::dispose(); }

int Application::run() {
  auto loop = core::Singleton<core::EventLoop>::instance();
  while (_running) {
    loop->nextTick();
    core::CoContext::yield();
  }
  return _exitcode;
}

void Application::exit(int exitcode) {
  auto loop = core::Singleton<core::EventLoop>::instance();
  _exitcode = exitcode;
  _running = false;
}