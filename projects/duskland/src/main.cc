

#include "core/AutoPtr.hpp"
#include "runtime/Application.hpp"
#include <exception>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace firefly;

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, NULL);
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
  try {
    core::AutoPtr app = new runtime::Application();
    return app->start(argc, argv);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception" << std::endl;
  }
  return -1;
}