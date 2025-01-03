#include "runtime/Locale.hpp"
#include "core/AutoPtr.hpp"
#include "core/Properties.hpp"
#include "core/Singleton.hpp"
#include "runtime/Logger.hpp"
#include "runtime/Media.hpp"
#include <fmt/core.h>
using namespace firefly;
using namespace firefly::runtime;
void Locale::scan() {
  _languages.clear();
  auto media = core::Singleton<Media>::instance();
  auto indices = media->scan("lang");
  for (auto &index : indices) {
    _languages.pushBack(index.substr(6, index.length() - 11));
  }
}
const core::String_t Locale::i18n(const core::String_t &key) {
  if (_locales->has(key)) {
    return _locales->get(key);
  }
  if (_default->has(key)) {
    return _default->get(key);
  }
  return key;
}
void Locale::setLang(const core::String_t &lang) {
  auto media = core::Singleton<Media>::instance();
  try {
    core::AutoPtr locales = new core::Properties(
        media->load(fmt::format("lang::{}.lang", lang))->read());
    if (_lang == lang) {
      _locales->merge(locales);
    } else {
      _locales = locales;
      _lang = lang;
    }
  } catch (std::exception &e) {
    auto logger = core::Singleton<Logger>::instance();
    logger->warn("Failed to set locale '{}':\n\t{}", lang, e.what());
  }
}
void Locale::setDefaultLang(const core::String_t &lang) {
  auto media = core::Singleton<Media>::instance();
  try {
    _defaultLang = lang;
    core::AutoPtr locales = new core::Properties(
        media->load(fmt::format("lang::{}.lang", lang))->read());
    _default = locales;
  } catch (std::exception &e) {
    auto logger = core::Singleton<Logger>::instance();
    logger->warn("Failed to set defualt locale '{}':\n\t{}", lang, e.what());
  }
}
void Locale::reload() {
  scan();
  setDefaultLang(_defaultLang);
  setLang(_lang);
}
const core::String_t &Locale::getLang() const { return _lang; }
const core::Array<core::String_t> &Locale::getLanguages() const {
  return _languages;
}