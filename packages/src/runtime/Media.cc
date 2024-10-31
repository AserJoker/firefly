#include "runtime/Media.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/File.hpp"
#include "runtime/MediaNotExistException.hpp"
#include "runtime/Resource.hpp"
#include <filesystem>
#include <fmt/core.h>

using namespace firefly;
using namespace firefly::runtime;
core::AutoPtr<Resource> Media::load(const core::String_t &name) {
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
core::Array<core::AutoPtr<Resource>>
Media::loadAll(const core::String_t &name) {
  auto filepaths = resolve(name);
  core::Array<core::AutoPtr<Resource>> result;
  for (auto it = filepaths.begin(); it != filepaths.end(); it++) {
    auto filepath = *it;
    if (std::filesystem::exists(filepath)) {
      result.pushBack(new ResourceTrait<core::File>(filepath));
    }
  }
  return result;
}
core::Array<core::String_t> Media::scan(const core::String_t &name) {
  core::Array<core::String_t> result;
  auto filepaths = resolve(name);
  for (auto &part : filepaths) {
    if (std::filesystem::exists(part)) {
      for (auto &item : std::filesystem::directory_iterator(part)) {
        auto key = fmt::format("{}::{}", name, item.path().filename().string());
        if (std::find(result.begin(), result.end(), key) == result.end()) {
          result.pushBack(key);
        }
      }
    }
  }
  return result;
}
core::Array<core::String_t> Media::resolve(const core::String_t &name) {
  core::Array<core::String_t> result;
  for (auto &_cwd : _cwds) {
    std::filesystem::path filepath = _cwd;
    core::String_t tmp = name;
    while (!tmp.empty()) {
      auto pos = tmp.find_first_of("::");
      if (pos == core::String_t::npos) {
        filepath.append(tmp);
        break;
      }
      auto part = tmp.substr(0, pos);
      tmp = tmp.substr(pos + 2);
      filepath.append(part);
    }
    if (std::filesystem::exists(filepath)) {
      result.pushBack(filepath.string());
    }
  }
  return result;
}
void Media::addCurrentWorkspaceDirectory(const core::String_t &cwd) {
  core::String_t _cwd;
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
  _cwds.pushBack(_cwd);
}