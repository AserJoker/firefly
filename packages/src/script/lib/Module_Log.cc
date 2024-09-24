#include "script/lib/Module_Log.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "exception/ValidateException.hpp"
#include "runtime/Logger.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
void Module_Log::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setField(ctx, "debug", ctx->createValue()->setFunction(ctx, debug));
  exports->setField(ctx, "info", ctx->createValue()->setFunction(ctx, info));
  exports->setField(ctx, "log", ctx->createValue()->setFunction(ctx, log));
  exports->setField(ctx, "warn", ctx->createValue()->setFunction(ctx, warn));
  exports->setField(ctx, "error", ctx->createValue()->setFunction(ctx, error));
  exports->setField(ctx, "panic", ctx->createValue()->setFunction(ctx, panic));
  ctx->registerModule("log", exports);
}
FUNC_DEF(Module_Log::debug) {
  VALIDATE_ARGS(debug, 1);
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->debug("{}", args[0]->toString(ctx));
  return {};
}
FUNC_DEF(Module_Log::info) {
  VALIDATE_ARGS(info, 1);
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->info("{}", args[0]->toString(ctx));
  return {};
}
FUNC_DEF(Module_Log::log) {
  VALIDATE_ARGS(log, 1);
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->log("{}", args[0]->toString(ctx));
  return {};
}
FUNC_DEF(Module_Log::warn) {
  VALIDATE_ARGS(warn, 1);
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->warn("{}", args[0]->toString(ctx));
  return {};
}
FUNC_DEF(Module_Log::error) {
  VALIDATE_ARGS(error, 1);
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->error("{}", args[0]->toString(ctx));
  return {};
}
FUNC_DEF(Module_Log::panic) {
  VALIDATE_ARGS(panic, 1);
  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->panic("{}", args[0]->toString(ctx));
  return {};
}