#pragma once
#include "Constant.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <unordered_map>
namespace firefly::video {
class Shader : public core::Object {
private:
  std::unordered_map<std::string, std::string> _sources;

private:
  static const std::unordered_map<std::string, std::string>
  load(const std::string &path);
  const static std::string preCompile(const std::string &source);

  static std::unordered_map<std::string, core::AutoPtr<Shader>> _cache;

public:
  static void clearCache();
  static core::AutoPtr<Shader> get(const std::string &name);

public:
  Shader(const std::string &path);
  Shader(const std::unordered_map<std::string, std::string> &sources);
  void setSource(const std::string &name, const std::string &source);
  const std::unordered_map<std::string, std::string> &getSources() const;
  core::AutoPtr<Constant> &getConstants();
  const core::AutoPtr<Constant> &getConstants() const;
};
}; // namespace firefly::video