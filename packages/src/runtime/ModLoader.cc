#include "runtime/ModLoader.hpp"
#include "core/AutoPtr.hpp"
#include "core/File.hpp"
#include "core/JSONException.hpp"
#include "core/Singleton.hpp"
#include "runtime/Logger.hpp"
#include "runtime/Media.hpp"
#include "runtime/ModLoaderException.hpp"
#include "runtime/Resource.hpp"
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

ModLoader::Manifest ModLoader::parseManifest(const std::string &source) {
  path modPath = source;
  path maniefstPath = modPath;
  Manifest manifest;
  core::AutoPtr file = new ResourceTrait<core::File>(
      maniefstPath.append("manifest.json").string());
  auto buffer = file->read();
  auto root =
      cJSON_ParseWithLength((const char *)buffer->getData(), buffer->getSize());
  if (!root) {
    throw core::JSONException(fmt::format("Failed to load manifest from '{}':\n\t {}",
                                    source, cJSON_GetErrorPtr()));
  }
  manifest.name = modPath.filename().string();
  manifest.path = source;
  manifest.loaded = false;
  auto child = root->child;
  while (child) {
    std::string key = child->string;
    if (key == "name") {
      manifest.displayName = child->valuestring;
    }
    if (key == "version") {
      manifest.version = Version::parse(child->valuestring);
    }
    if (key == "author") {
      manifest.author = child->valuestring;
    }
    if (key == "email") {
      manifest.author = child->valuestring;
    }
    if (key == "icon") {
      manifest.icon = child->valuestring;
    }
    if (key == "preview") {
      manifest.preview = child->valuestring;
    }
    if (key == "description") {
      manifest.description = child->valuestring;
    }
    if (key == "dependences") {
      auto dep = child->child;
      while (dep) {
        std::string name;
        Version version;
        auto depField = dep->child;
        while (depField) {
          std::string key = depField->string;
          if (key == "name") {
            name = depField->valuestring;
          }
          if (key == "version") {
            version = Version::parse(depField->valuestring);
          }
          depField = depField->next;
        }
        manifest.dependences[name] = version;
        dep = dep->next;
      }
    }
    if (key == "runtimeDependences") {
      auto dep = child->child;
      while (dep) {
        std::string name;
        Version version;
        auto depField = dep->child;
        while (depField) {
          std::string key = depField->string;
          if (key == "name") {
            name = depField->valuestring;
          }
          if (key == "version") {
            version = Version::parse(depField->valuestring);
          }
          depField = depField->next;
        }
        manifest.runtimeDependences[name] = version;
        dep = dep->next;
      }
    }
    child = child->next;
  }
  cJSON_free(root);
  return manifest;
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