#include "script/lib/Module_Locale.hpp"
#include "core/Singleton.hpp"
#include "runtime/Locale.hpp"
using namespace firefly;
using namespace firefly::script;

FUNC_DEF(Module_Locale::i18n) {
  auto locale = core::Singleton<runtime::Locale>::instance();
  return {
      ctx->createValue()->setString(ctx, locale->i18n(args[0]->toString(ctx)))};
}
FUNC_DEF(Module_Locale::setLang) {
  auto locale = core::Singleton<runtime::Locale>::instance();
  locale->setLang(args[0]->toString(ctx));
  return {};
}
FUNC_DEF(Module_Locale::getLang) {
  auto locale = core::Singleton<runtime::Locale>::instance();
  return {ctx->createValue()->setString(ctx, locale->getLang())};
}
FUNC_DEF(Module_Locale::getLanguages) {
  auto locale = core::Singleton<runtime::Locale>::instance();
  auto languages = locale->getLanguages();
  auto result = ctx->createValue()->setArray(ctx);
  for (auto i = 0; i < languages.size(); i++) {
    result->setIndex(ctx, i, ctx->createValue()->setString(ctx, languages[i]));
  }
  return {result};
}

void Module_Locale::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setField(ctx, "i18n", ctx->createValue()->setFunction(ctx, i18n));
  exports->setField(ctx, "setLang",
                    ctx->createValue()->setFunction(ctx, setLang));
  exports->setField(ctx, "getLang",
                    ctx->createValue()->setFunction(ctx, getLang));
  exports->setField(ctx, "getLanguages",
                    ctx->createValue()->setFunction(ctx, getLanguages));
  ctx->registerModule("locale", exports);
}