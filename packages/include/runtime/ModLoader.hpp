#pragma once
#include "core/Array.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include <fmt/format.h>


namespace firefly::runtime {
class ModLoader : public core::Object {
public:
  struct Version {
    int major;
    int minor;
    int patch;
    const core::Boolean_t operator>=(const Version &another) const {
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
    const core::Boolean_t operator<(const Version &another) const {
      return !(*this >= another);
    }
    const core::String_t toString() const {
      return fmt::format("v{}.{}.{}", major, minor, patch);
    }
    static Version parse(const core::String_t &src) {
      core::CString_t s = src.c_str();
      if (*s == 'v') {
        s++;
      }
      Version v;
      core::String_t tmp;
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
    core::String_t name;
    core::String_t displayName;
    Version version;
    core::Map<core::String_t, Version> dependences;
    core::Map<core::String_t, Version> runtimeDependences;
    core::String_t icon;
    core::String_t preview;
    core::String_t description;
    core::String_t author;
    core::String_t email;
    core::String_t path;
    core::Boolean_t loaded;
  };

private:
  core::Map<core::String_t, Manifest> _mods;

private:
  Manifest parseManifest(const core::String_t &source);
  void load(core::Map<core::String_t, Manifest> &workflow, Manifest &manifest,
            const core::Array<core::String_t> &requirePath = {});

public:
  void loadAll(const core::String_t &root);
  void loadMod(const core::String_t &name);
  void scan(const core::String_t &root);
  const core::Map<core::String_t, Manifest> &getMods() const;
  const Manifest *getManifest(const core::String_t &name) const;
};
}; // namespace firefly::runtime