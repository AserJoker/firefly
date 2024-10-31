#pragma once
#include "Resource.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"


namespace firefly::runtime {
class Media : public core::Object {

private:
  core::Array<core::String_t> _cwds;

public:
  core::AutoPtr<Resource> load(const core::String_t &name);
  core::Array<core::AutoPtr<Resource>> loadAll(const core::String_t &name);
  core::Array<core::String_t> scan(const core::String_t &name);
  void addCurrentWorkspaceDirectory(const core::String_t &cwd);
  core::Array<core::String_t> resolve(const core::String_t &name);
};
} // namespace firefly::runtime