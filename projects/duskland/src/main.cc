#include "GameApplication.hpp"
#include "firefly.hpp"
#include <SDL2/SDL.h>
#include <exception>
using namespace duskland;
using namespace firefly;

int main(int argc, char *argv[]) {
  InitFirefly();
  try {
    core::Singleton<runtime::Application>::initialize<GameApplication>(argc,
                                                                       argv);
    return core::Singleton<runtime::Application>::instance()->run();
  } catch (std::exception &e) {
    auto &theLogger = core::Singleton<runtime::Logger>::instance();
    theLogger->panic("{}", e.what());
  }
  return 0;
}