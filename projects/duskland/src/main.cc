#include "core/AutoPtr.hpp"
#include "core/Coroutine.hpp"
#include "core/EventLoop.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "runtime/Logger.hpp"
#include <iostream>
using namespace firefly;
void f1() {
  for (int i = 0; i < 10; i++) {
    std::cout << "f1:" << i << std::endl;
    core::CoContext::yield();
  }
}
void f2() {
  for (int i = 0; i < 10; i++) {
    std::cout << "f2:" << i << std::endl;
    core::CoContext::yield();
  }
}
void onMainLoop() {
  auto &theLoop = core::Singleton<core::EventLoop>::instance();
  auto &theApp = core::Singleton<runtime::Application>::instance();
  if (!core::CoContext::ready()) {
    theLoop->start(onMainLoop);
  } else {
    theApp->exit();
  }
}
int main(int argc, char *argv[]) {
  auto &theApp = core::Singleton<runtime::Application>::instance();
  theApp = new runtime::Application(argc, argv);
  auto &theLoop = core::Singleton<core::EventLoop>::instance();
  theLoop->start(onMainLoop);
  auto &theLogger = core::Singleton<runtime::Logger>::instance();
  theLogger = new runtime::Logger();
  theLogger->info("hello world {}", "firefly");
  // core::CoContext::start(std::function(f1));
  // core::CoContext::start(std::function(f2));
  return theApp->run();
}