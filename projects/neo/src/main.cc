
#include "script/engine/JSContext.hpp"
#include "script/engine/JSExceptionType.hpp"
// #include <SDL2/SDL.h>
#include <codecvt>
#include <exception>
#include <iostream>
#include <locale>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

JSValue *print(JSContext *ctx, JSValue *self, std::vector<JSValue *> args) {
  for (auto &arg : args) {
    auto str = ctx->toString(arg);
    if (str->isTypeof<JSExceptionType>()) {
      return str;
    }
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
  try {
    auto runtime = new JSRuntime(argc, argv);
    auto ctx = new JSContext(runtime);
    auto obj = ctx->createObject(ctx->createNull());
    auto num = ctx->createNumber(123);
    ctx->setField(obj, ctx->createString(L"test"), num);
    ctx->getField(obj, ctx->createString(L"test"));
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