#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
using namespace firefly;
int main(int argc, char *argv[]) {
  auto &theApp = core::Singleton<runtime::Application>::instance();
  theApp = new runtime::Application(argc, argv);
  return theApp->run();
}