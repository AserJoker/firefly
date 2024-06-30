#include "core/AutoPtr.hpp"
#include "core/Coroutine.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
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
int main(int argc, char *argv[]) {
  auto &theApp = core::Singleton<runtime::Application>::instance();
  theApp = new runtime::Application(argc, argv);
  core::CoContext::start(std::function(f1));
  core::CoContext::start(std::function(f2));
  return theApp->run();
}