
#include "script/neo.hpp"
#include <exception>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace neo;

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, NULL);
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
  try {
    std::wifstream file("../script/index.js",std::ios::in|std::ios::binary);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    wchar_t *buf = new wchar_t[size];
    file.read(buf, size);
    file.close();
    std::wstring js(buf, size);
    auto runtime = new JSRuntime();
    auto ctx = new JSContext(runtime);
    ctx->eval(L"./script/index.js", js);
    delete ctx;
    delete runtime;
    return 0;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception" << std::endl;
  }
  return -1;
}