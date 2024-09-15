#define _CRT_SECURE_NO_WARNINGS
#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "firefly.hpp"
#include <SDL2/SDL.h>
#include <clocale>
#include <exception>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace duskland;
using namespace firefly;

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, NULL);
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
  try {
    InitFirefly();
    core::Singleton<runtime::Application>::initialize<GameApplication>(argc,
                                                                       argv);
    return core::Singleton<runtime::Application>::instance()->run();
  } catch (std::exception &e) {
    auto &theLogger = core::Singleton<runtime::Logger>::instance();
    theLogger->panic("{}", e.what());
  } catch (...) {
    std::cerr << "Unknown error" << std::endl;
  }
  return 0;
}
