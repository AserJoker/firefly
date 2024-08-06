#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "firefly.hpp"
#include "script/Context.hpp"
#include "script/Runtime.hpp"
#include "script/Value.hpp"
#include <SDL2/SDL.h>
#include <exception>
#include <iostream>
using namespace duskland;
using namespace firefly;

struct Lua_trait {
  static void eval(const std::string &filename, const std::string &source) {}
};

int main(int argc, char *argv[]) {
  InitFirefly();
  core::AutoPtr runtime = new script::AdapterRuntime<Lua_trait>();
  core::AutoPtr ctx = new script::Context(runtime);
  auto meta = ctx->createValue()->setObject();
  meta->setField(
      ctx, "gc",
      ctx->createValue()->setFunction(
          [](core::AutoPtr<script::Context> ctx,
             script::Value::Stack args) -> script::Value::Stack {
            std::cout << "gc" << std::endl;
            ctx->getGlobal()->setField(ctx, "data", args[0]);
            args[0]
                ->getField(ctx, "print")
                ->call(ctx, {ctx->createValue()->setString("Hello world")});
            return {};
          }));
  auto scope = ctx->pushScope();
  auto data = ctx->createValue()->setMetadata(meta)->setObject()->setField(
      ctx, "print",
      ctx->createValue()->setFunction(
          [](core::AutoPtr<script::Context> ctx,
             script::Value::Stack args) -> script::Value::Stack {
            std::cout << args[0]->toString() << std::endl;
            return {};
          }));
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
