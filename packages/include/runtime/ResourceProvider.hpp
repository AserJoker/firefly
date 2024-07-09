#pragma once
#include "Resource.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::runtime {
class ResourceProvider : public core::Object {
private:
  std::string _cwd;

public:
  core::AutoPtr<Resource> load(const std::string &name);
  void dump(const std::string &name, core::AutoPtr<Resource> resource);
  void setCurrentWorkspaceDirectory(const std::string &cwd);
  std::string resolve(const std::string &name);
};
} // namespace firefly::runtime