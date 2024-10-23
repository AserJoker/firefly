#include "runtime/Media.hpp"
#include "core/AutoPtr.hpp"
#include "core/File.hpp"
#include "runtime/MediaNotExistException.hpp"
#include "runtime/Resource.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <vector>

using namespace firefly;
using namespace firefly::runtime;
core::AutoPtr<Resource> Media::load(const std::string &name) {
  auto filepaths = resolve(name);
  for (auto it = filepaths.rbegin(); it != filepaths.rend(); it++) {
    auto filepath = *it;
    if (std::filesystem::exists(filepath)) {
      return new ResourceTrait<core::File>(filepath);
    }
  }
  throw MediaNotExistException(
      fmt::format("Failed to load resource: {}", name));
}
std::vector<core::AutoPtr<Resource>> Media::loadAll(const std::string &name) {
  auto filepaths = resolve(name);
  std::vector<core::AutoPtr<Resource>> result;
  for (auto it = filepaths.begin(); it != filepaths.end(); it++) {
    auto filepath = *it;
    if (std::filesystem::exists(filepath)) {
      result.push_back(new ResourceTrait<core::File>(filepath));
    }
  }
  return result;
}
std::vector<std::string> Media::scan(const std::string &name) {
  std::vector<std::string> result;
  auto filepaths = resolve(name);
  for (auto &part : filepaths) {
    if (std::filesystem::exists(part)) {
      for (auto &item : std::filesystem::directory_iterator(part)) {
        auto key = fmt::format("{}::{}", name, item.path().filename().string());
        if (std::find(result.begin(), result.end(), key) == result.end()) {
          result.push_back(key);
        }
      }
    }
  }
  return result;
}
std::vector<std::string> Media::resolve(const std::string &name) {
  std::vector<std::string> result;
  for (auto &_cwd : _cwds) {
    std::filesystem::path filepath = _cwd;
    std::string tmp = name;
    while (!tmp.empty()) {
      auto pos = tmp.find_first_of("::");
      if (pos == std::string::npos) {
        filepath.append(tmp);
        break;
      }
      auto part = tmp.substr(0, pos);
      tmp = tmp.substr(pos + 2);
      filepath.append(part);
    }
    if (std::filesystem::exists(filepath)) {
      result.push_back(filepath.string());
    }
  }
  return result;
}
void Media::addCurrentWorkspaceDirectory(const std::string &cwd) {
  std::string _cwd;
  for (auto &c : cwd) {
    if (c == '\\') {
      _cwd += '/';
    } else {
      _cwd += c;
    }
  }
  if (!_cwd.ends_with('/')) {
    _cwd += '/';
  }
  _cwds.push_back(_cwd);
}