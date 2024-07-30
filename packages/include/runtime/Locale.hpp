#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Properties.hpp"
#include <string>
#include <vector>
namespace firefly::runtime {
class Locale : public core::Object {
private:
  core::AutoPtr<core::Properties> _locales;
  core::AutoPtr<core::Properties> _default;
  std::vector<std::string> _languages;
  std::string _lang;
  std::string _defaultLang;

public:
  void scan();
  const std::string i18n(const std::string &key);
  void setLang(const std::string &lang);
  void setDefaultLang(const std::string &lang);
  void reload();
  const std::string &getLang() const;
  const std::vector<std::string> &getLanguages() const;
};
} // namespace firefly::runtime