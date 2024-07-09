#include "runtime/ResourceProvider.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Resource.hpp"
#include "runtime/Resource_File.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::runtime;
core::AutoPtr<Resource> ResourceProvider::load(const std::string &name) {
  auto filepath = resolve(name);
  if (!std::filesystem::exists(filepath)) {
    throw std::runtime_error(fmt::format("Failed to load resource: {}", name));
  }
  return new Resource_File(resolve(name));
}
void ResourceProvider::dump(const std::string &name,
                            core::AutoPtr<Resource> data) {
  auto filepath = resolve(name);
  core::AutoPtr res = new Resource_File(filepath);
  size_t len = 0;
  for (;;) {
    auto chunk = data->read(128);
    if (chunk->getSize()) {
      res->write(chunk);
    } else {
      break;
    }
  }
}
std::string ResourceProvider::resolve(const std::string &name) {
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
  return filepath.string();
}
void ResourceProvider::setCurrentWorkspaceDirectory(const std::string &cwd) {
  _cwd = cwd;
}