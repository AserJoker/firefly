
#include "script/neo.hpp"
#include <codecvt>
#include <exception>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace neo;

JSValue *print(JSContext *ctx, JSValue *self, std::vector<JSValue *> args) {
  for (auto &arg : args) {
    std::wcout << ctx->checkedString(ctx->toString(arg)) << std::endl;
  }
  return ctx->createNumber(args.size());
}
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
      throw std::runtime_error(fmt::format("Failed to open file:{}", filename));
    }
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    wchar_t *buf = new wchar_t[size];
    file.read(buf, size);
    file.close();
    std::wstring js(buf, size);
    delete[] buf;
    auto runtime = new JSRuntime();
    auto ctx = new JSContext(runtime);
    auto func = ctx->createNativeFunction(print, L"print");
    auto global = ctx->getGlobal();
    ctx->setField(global, L"print", func);
    ctx->eval(L"../script/index.js", js);
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