#include "video/Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Media.hpp"
#include <filesystem>
#include <unordered_map>
using namespace firefly;
using namespace firefly::video;

const std::unordered_map<std::string, std::string>
Shader::load(const std::string &path) {
  std::unordered_map<std::string, std::string> sources;
  core::AutoPtr media = core::Singleton<runtime::Media>::instance();
  auto items = media->scan(path);
  for (auto &item : items) {
    auto files = media->resolve(item);
    if (!files.empty()) {
      std::filesystem::path itemPath = files[files.size() - 1];
      if (!std::filesystem::is_directory(itemPath) &&
          itemPath.extension() == ".glsl") {
        auto name = itemPath.filename().string();
        name = name.substr(0, name.length() - 5);
        auto buffer = media->load(item)->read();
        sources[name] = preCompile(
            std::string((const char *)buffer->getData(), buffer->getSize()));
      }
    }
  }
  return sources;
}
Shader::Shader(const std::string &path) : Shader(load(path)) {}
Shader::Shader(const std::unordered_map<std::string, std::string> &sources)
    : _sources(sources) {
  setVersion(getVersion() + 1);
}
void Shader::setSource(const std::string &name, const std::string &source) {
  _sources[name] = preCompile(source);
  setVersion(getVersion() + 1);
}
const std::string Shader::preCompile(const std::string &source) {
  std::string result = source;
  return result;
}
const std::unordered_map<std::string, std::string> &Shader::getSources() const {
  return _sources;
}