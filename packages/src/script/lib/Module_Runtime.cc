#include "script/lib/Module_Runtime.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
using namespace firefly;
using namespace firefly::script;
void Module_Runtime::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setFunctionField(ctx, setWindowTitle);
  exports->setFunctionField(ctx, getWindowTitle);
  exports->setFunctionField(ctx, setWindowSize);
  exports->setFunctionField(ctx, getWindowSize);
  ctx->registerModule("runtime", exports);
}
FUNC_DEF(Module_Runtime::setWindowTitle) {
  auto title = args[0]->toString(ctx);
  auto app = core::Singleton<runtime::Application>::instance();
  app->getWindow()->setTitle(title);
  return {};
}
FUNC_DEF(Module_Runtime::getWindowTitle) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto title = app->getWindow()->getTitle();
  return {ctx->createValue()->setString(ctx, title)};
}
FUNC_DEF(Module_Runtime::setWindowSize) {
  uint32_t width = args[0]->toNumber(ctx);
  uint32_t height = args[1]->toNumber(ctx);
  auto app = core::Singleton<runtime::Application>::instance();
  app->getWindow()->setSize({width, height});
  return {};
}
FUNC_DEF(Module_Runtime::getWindowSize) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto size = app->getWindow()->getSize();
  return {ctx->createValue()->setNumber(ctx, size.x),
          ctx->createValue()->setNumber(ctx, size.y)};
}