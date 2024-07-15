#pragma once
#include "Resource.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <vector>
#ifdef _MSVC_LANG
#include <vcruntime_typeinfo.h>
#else
#include <typeinfo>
#endif
#include <unordered_map>

namespace firefly::runtime {
class ResourceProvider : public core::Object {
  using named_resource =
      std::unordered_map<std::string, core::AutoPtr<core::Object>>;

private:
  std::string _cwd;
  std::unordered_map<std::string, named_resource> _resources;

public:
  core::AutoPtr<Resource> load(const std::string &name);
  std::vector<std::string> index(const std::string &name);
  void dump(const std::string &name, core::AutoPtr<Resource> resource);
  void setCurrentWorkspaceDirectory(const std::string &cwd);
  std::string resolve(const std::string &name);
  template <class T> core::AutoPtr<T> query(const std::string &name) {
    auto type_name = typeid(T).name();
    auto &named = _resources[type_name];
    if (!named.contains(name)) {
      named[name] = new T(load(name));
    }
    return named.at(name).cast<T>();
  }
};
} // namespace firefly::runtime