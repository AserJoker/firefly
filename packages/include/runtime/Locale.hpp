#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Properties.hpp"

namespace firefly::runtime {
class Locale : public core::Object {
private:
  core::AutoPtr<core::Properties> _locales;
  core::AutoPtr<core::Properties> _default;
  core::Array<core::String_t> _languages;
  core::String_t _lang;
  core::String_t _defaultLang;

public:
  void scan();
  const core::String_t i18n(const core::String_t &key);
  void setLang(const core::String_t &lang);
  void setDefaultLang(const core::String_t &lang);
  void reload();
  const core::String_t &getLang() const;
  const core::Array<core::String_t> &getLanguages() const;
};
} // namespace firefly::runtime