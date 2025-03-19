
#include "script/engine/JSContext.hpp"
// #include <SDL2/SDL.h>
#include <codecvt>
#include <exception>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

JSValue *print(JSContext *ctx, JSValue *self, std::vector<JSValue *> args) {
  for (auto &arg : args) {
    auto str = ctx->toString(ctx->pack(arg));
    CHECK(ctx, str);
    std::wcout << ctx->checkedString(str) << std::endl;
  }
  return ctx->createUndefined();
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
  std::wifstream file(filename, std::ios::binary | std::ios::in);
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  file.seekg(0, std::ios::beg);
  auto buf = new wchar_t[size + 1];
  file.read(buf, size);
  file.close();
  buf[size] = 0;
  std::wstring source = buf;
  delete[] buf;
  try {
    auto runtime = new JSRuntime(argc, argv);
    auto ctx = new JSContext(runtime);
    ctx->setField(ctx->getGlobal(), ctx->createString(L"print"),
                  ctx->createNativeFunction(print, L"print"));
    auto res = ctx->eval(L"../script/index.js", source);
    std::wcout << res->getType()->getTypeName()<<":"
               << ctx->checkedString(ctx->toString(res)) << std::endl;
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