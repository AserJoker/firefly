#pragma once
#include "core/Object.hpp"
#include <fmt/format.h>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::runtime {
class ModLoader : public core::Object {
public:
  struct Version {
    int major;
    int minor;
    int patch;
    const bool operator>=(const Version &another) const {
      if (major > another.major) {
        return true;
      } else if (major == another.major) {
        if (minor > another.minor) {
          return true;
        } else if (minor == another.minor) {
          return patch >= another.patch;
        }
      }
      return false;
    }
    const bool operator<(const Version &another) const {
      return !(*this >= another);
    }
    const std::string toString() const {
      return fmt::format("v{}.{}.{}", major, minor, patch);
    }
    static Version parse(const std::string &src) {
      const char *s = src.c_str();
      if (*s == 'v') {
        s++;
      }
      Version v;
      std::string tmp;
      int *part = &v.major;
      while (*s) {
        if (*s == '.') {
          *part = atoi(tmp.c_str());
          tmp.clear();
          if (part == &v.major) {
            part = &v.minor;
          } else {
            part = &v.patch;
          }
        } else if (*s == '-') {
          break;
        } else {
          tmp += *s;
        }
        s++;
      }
      *part = atoi(tmp.c_str());
      return v;
    }
  };
  struct Manifest {
    std::string name;
    std::string displayName;
    Version version;
    std::unordered_map<std::string, Version> dependences;
    std::unordered_map<std::string, Version> runtimeDependences;
    std::string icon;
    std::string preview;
    std::string description;
    std::string author;
    std::string email;
    std::string path;
  };

private:
  std::unordered_map<std::string, Manifest> _mods;

private:
  Manifest parseManifest(const std::string &source);
  void load(std::unordered_map<std::string, Manifest> &workflow,
            std::unordered_map<std::string, Manifest> &cache,
            const Manifest &manifest,
            const std::vector<std::string> &requirePath = {});

public:
  void loadAll(const std::string &root);
  const Manifest *getManifest(const std::string &name) const;
};
}; // namespace firefly::runtime