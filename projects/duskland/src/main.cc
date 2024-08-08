#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "firefly.hpp"
#include "script/Context.hpp"
#include "script/Value.hpp"
#include <SDL2/SDL.h>
#include <exception>
#include <iostream>

using namespace duskland;
using namespace firefly;

int main(int argc, char *argv[]) {
  InitFirefly();
  core::AutoPtr ctx = new script::Context();
  auto meta = ctx->createValue()->setObject(ctx);
  meta->setField(ctx, "gc",
                 ctx->createValue()->setFunction(
                     ctx,
                     [](core::AutoPtr<script::Context> ctx,
                        script::Value::Stack args) -> script::Value::Stack {
                       auto self = args[0];
                       auto global = ctx->getGlobal();
                       global->setField(ctx, "data", self);
                       std::cout << "gc" << std::endl;
                       return {};
                     }));
  auto scope = ctx->pushScope();
  auto obj = ctx->createValue()->setObject(ctx)->setMetadata(meta);
  ctx->popScope(scope);
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
