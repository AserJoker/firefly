#pragma once
#include "script/Script.hpp"
namespace firefly::script {
class Module_Locale {
private:
  static FUNC_DEF(i18n);
  static FUNC_DEF(setLang);
  static FUNC_DEF(getLang);
  static FUNC_DEF(getLanguages);

public:
  static void open(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script