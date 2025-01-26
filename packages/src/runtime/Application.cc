#include "runtime/Application.hpp"
#include "runtime/ClientProxy.hpp"
#include "runtime/RunProxy.hpp"
#include <clocale>
#include <string>

using namespace firefly;
using namespace firefly::runtime;
Application::Application() {}

int Application::start(int argc, char *argv[]) {
  for (auto index = 0; index < argc; index++) {
    _argv.push_back(argv[index]);
  }

  core::AutoPtr<RunProxy> proxy = core::Singleton<RunProxy>::instance();
  if (!proxy) {
    proxy = new ClientProxy();
  }
  return proxy->run();
}

const std::string &Application::argv(uint32_t index) const {
  static std::string empty = "";
  if (index < _argv.size()) {
    return _argv[index];
  }
  return empty;
}
