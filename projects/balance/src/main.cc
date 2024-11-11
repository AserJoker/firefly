#include "core/AutoPtr.hpp"
#include "core/Coroutine.hpp"
#include "runtime/Media.hpp"
#include "script/JSCompiler.hpp"
#include <codecvt>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
using namespace firefly;
int main(int argc, char *argv[]) {
  try {
    core::Coroutine::init();
    std::filesystem::path dir = argv[0];
    core::AutoPtr<script::JSCompiler> compiler = new script::JSCompiler();
    core::AutoPtr media = new runtime::Media();
    media->addCurrentWorkspaceDirectory(dir.parent_path().string());
    auto buf = media->load("main.mjs")->read();
    
    std::string source({(const char *)buf->getData(), buf->getSize()});
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    auto json = compiler->compile("main.mjs", source)
                    ->toJSON(converter.from_bytes(source));
    std::ofstream out("1.json");
    out << json;
    out.close();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}