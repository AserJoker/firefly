#pragma once
#include "Resource.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <vector>

namespace firefly::runtime {
class Media : public core::Object {

private:
  std::vector<std::string> _cwds;

public:
  core::AutoPtr<Resource> load(const std::string &name);
  std::vector<core::AutoPtr<Resource>> loadAll(const std::string &name);
  std::vector<std::string> scan(const std::string &name);
  void addCurrentWorkspaceDirectory(const std::string &cwd);
  std::vector<std::string> resolve(const std::string &name);
};
} // namespace firefly::runtime