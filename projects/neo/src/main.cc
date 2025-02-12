
#include "script/neo.hpp"
#include <codecvt>
#include <exception>
#include <fstream>
#include <iostream>
#include <locale>
#include <fmt/format.h>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace neo;

std::string filename = "../script/index.js";

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, NULL);
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  if (argc > 1) {
    filename = argv[1];
  }
  try {
    std::wifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error(fmt::format("Failed to open file:{}", argv[1]));
    }
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    wchar_t *buf = new wchar_t[size];
    file.read(buf, size);
    file.close();
    std::wstring js(buf, size);
    auto runtime = new JSRuntime();
    auto ctx = new JSContext(runtime);
    ctx->eval(converter.from_bytes(filename), js);
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