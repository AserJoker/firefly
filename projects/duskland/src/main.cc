#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "firefly.hpp"
#include "script/Context.hpp"
#include "script/LuaBridge.hpp"
#include "script/Value.hpp"
#include <SDL2/SDL.h>
#include <exception>
#include <iostream>

using namespace duskland;
using namespace firefly;

int main(int argc, char *argv[]) {
  InitFirefly();
  core::Singleton<script::Context::Bridge>::initialize<script::LuaBridge>();
  core::AutoPtr ctx = core::Singleton<script::Context>::instance();
  auto scope = ctx->pushScope();
  ctx->registerModule(
      "native",
      {{"print", ctx->createValue()->setFunction(
                     ctx,
                     [](core::AutoPtr<script::Context> ctx,
                        script::Value::Stack args) -> script::Value::Stack {
                       std::cout << "hello world" << std::endl;
                       return {};
                     })}});
  ctx->popScope(scope);
  auto bridge = ctx->getBridge().cast<script::LuaBridge>();
  bridge->eval("init.lua", "");
  return 0;
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
