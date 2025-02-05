
#include "script/neo.hpp"
#include <exception>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

auto js = LR"(#!/usr/bin/node
"use strict"
/*aaaa
)";

using namespace neo;

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, NULL);
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
  try {
    auto runtime = new JSRuntime();
    auto ctx = new JSContext(runtime);
    ctx->eval(L"index.js", js);
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