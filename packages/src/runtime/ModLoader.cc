#include "runtime/ModLoader.hpp"
#include "core/AutoPtr.hpp"
#include "core/File.hpp"
#include "core/Json.hpp"
#include "core/Singleton.hpp"
#include "core/Value.hpp"
#include "runtime/Logger.hpp"
#include "runtime/Media.hpp"
#include "runtime/ModLoaderException.hpp"
#include <algorithm>
#include <cjson/cJSON.h>
#include <exception>
#include <filesystem>
#include <unordered_map>

using namespace std::filesystem;
using namespace firefly;
using namespace firefly::runtime;
void ModLoader::load(std::unordered_map<std::string, Manifest> &workflow,
                     Manifest &manifest,
                     const std::vector<std::string> &requirePath) {
  for (auto &[name, version] : manifest.dependences) {
    if (std::find(requirePath.begin(), requirePath.end(), name) !=
        requirePath.end()) {
      std::string path;
      for (auto &part : requirePath) {
        if (!path.empty()) {
          path += "->";
        }
        path += part;
      }
      path += "->" + name;
      throw ModLoaderException(fmt::format(
          "Failed to load mod '{}':\n\t cycle dependices find:\n\t{}",
          manifest.name, path));
    }
    Manifest dep;
    if (workflow.contains(name)) {
      dep = workflow.at(name);
      workflow.erase(name);
    } else if (_mods.contains(name)) {
      dep = _mods.at(name);
    } else {
      throw ModLoaderException(
          fmt::format("Failed to load mod '{}',dependence '{}' is not found",
                      manifest.displayName, name));
    }
    if (version >= dep.version) {
      throw ModLoaderException(
          fmt::format("Failed to load mod '{}',dependence '{}' is too "
                      "old,need '{}',given '{}'",
                      manifest.displayName, name, version.toString(),
                      dep.version.toString()));
    }
    if (!dep.loaded) {
      auto rpath = requirePath;
      rpath.push_back(manifest.name);
      load(workflow, dep, rpath);
    }
  }
  path modPath = manifest.path;
  path mediaPath = modPath;
  mediaPath.append("media");
  path luaPath = modPath;
  luaPath.append("lua");
  if (exists(mediaPath)) {
    auto media = core::Singleton<Media>::instance();
    media->addCurrentWorkspaceDirectory(mediaPath.string());
  }
  if (exists(luaPath)) {
    // TODO: run script
  }
  manifest.loaded = true;
}
void ModLoader::scan(const std::string &root) {
  auto logger = core::Singleton<Logger>::instance();
  _mods.clear();
  for (auto item : directory_iterator(root)) {
    auto path = item.path();
    if (item.is_directory() && exists(path.append("manifest.json"))) {
      try {
        Manifest manifest = parseManifest(item.path().string());
        _mods[manifest.name] = manifest;
      } catch (std::exception &e) {
        logger->warn("Failed to load manifest from '{}':\n\t{}",
                     item.path().string(), e.what());
      }
    }
  }
}
void ModLoader::loadAll(const std::string &root) {
  scan(root);
  std::unordered_map<std::string, Manifest> workflow = _mods;
  std::unordered_map<std::string, Manifest> cache;
  while (!workflow.empty()) {
    Manifest manifest = workflow.begin()->second;
    workflow.erase(manifest.name);
    load(workflow, manifest);
  }
  for (auto &[_, manifest] : _mods) {
    for (auto &[name, version] : manifest.runtimeDependences) {
      if (!_mods.contains(name)) {
        throw ModLoaderException(fmt::format(
            "Failed to load mod '{}',runtime dependence '{}' is not found",
            manifest.displayName, name));
      }
      if (_mods.at(name).version < version) {
        throw ModLoaderException(fmt::format(
            "Failed to load mod '{}',runtime dependence '{}' is too "
            "old,need '{}',given '{}'",
            manifest.displayName, name, version.toString(),
            _mods.at(name).version.toString()));
      }
    }
  }
}

template <class T>
static void parseField(T &value, core::Map_t &root, const std::string &field) {
  try {
    if (root.contains(field)) {
      value = root.at(field).get<T>();
    }
  } catch (...) {
    throw ModLoaderException(fmt::format("invalid field '{}'", field));
  }
}

ModLoader::Manifest ModLoader::parseManifest(const std::string &source) {
  try {
    path modPath = source;
    path manifestPath = modPath;
    Manifest manifest;
    core::AutoPtr file =
        new core::File(manifestPath.append("manifest.json").string());

    auto buffer = file->read();

    auto json = core::Json::parse(
        std::string((char *)buffer->getData(), buffer->getSize()));
    if (json.getType() != core::ValueType::MAP) {
      throw ModLoaderException("invalid format");
    }
    auto &map = json.get<core::Map_t>();
    manifest.name = modPath.filename().string();
    manifest.path = source;
    manifest.loaded = false;
    std::string version;
    parseField(manifest.displayName, map, "displayName");
    parseField(version, map, "version");
    manifest.version = Version::parse(version);
    parseField(manifest.author, map, "author");
    parseField(manifest.email, map, "email");
    parseField(manifest.icon, map, "icon");
    parseField(manifest.preview, map, "preview");
    parseField(manifest.description, map, "description");
    if (map.contains("dependences")) {
      try {
        auto &arr = map["dependences"].get<core::Array_t>();
        for (auto &item : arr) {
          auto &dep = item.get<core::Map_t>();
          auto &name = dep["name"].get<core::String_t>();
          auto &version = dep["version"].get<core::String_t>();
          manifest.dependences[name] = Version::parse(version);
        }
      } catch (...) {
        throw ModLoaderException("invalid format 'dependences'");
      }
    }
    if (map.contains("runtimeDependences")) {
      try {
        auto &arr = map["runtimeDependences"].get<core::Array_t>();
        for (auto &item : arr) {
          auto &dep = item.get<core::Map_t>();
          auto &name = dep["name"].get<core::String_t>();
          auto &version = dep["version"].get<core::String_t>();
          manifest.runtimeDependences[name] = Version::parse(version);
        }
      } catch (...) {
        throw ModLoaderException("invalid format 'runtimeDependences'");
      }
    }
    return manifest;
  } catch (std::exception &e) {
    throw ModLoaderException(
        fmt::format("Failed to resolve manifest.json: {}", e.what()));
  }
}

void ModLoader::loadMod(const std::string &name) {
  if (_mods.contains(name)) {
    Manifest &manifest = _mods.at(name);
    std::unordered_map<std::string, Manifest> workflow;
    load(workflow, manifest, {name});
  }
}

const std::unordered_map<std::string, ModLoader::Manifest> &
ModLoader::getMods() const {
  return _mods;
}