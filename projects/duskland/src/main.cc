#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "firefly.hpp"
#include "script/LuaBridge.hpp"
#include "script/Script.hpp"
#include <SDL2/SDL.h>
#include <exception>

using namespace duskland;
using namespace firefly;

int main(int argc, char *argv[]) {
  core::Singleton<script::Script::Bridge>::initialize<script::LuaBridge>();
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
