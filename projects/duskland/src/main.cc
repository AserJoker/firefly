#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "runtime/Logger.hpp"
#include "runtime/OpenGLApplication.hpp"
#include <exception>
using namespace firefly;
int main(int argc, char *argv[]) {
  try {
    auto &theApp = core::Singleton<runtime::Application>::instance();
    theApp = new runtime::OpenGLApplication(argc, argv);
    return theApp->run();
  } catch (std::exception &e) {
    auto &theLogger = core::Singleton<runtime::Logger>::instance();
    theLogger->panic("{}", e.what());
  }
}