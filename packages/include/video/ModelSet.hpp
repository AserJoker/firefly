#pragma once
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "video/Model.hpp"
#include <unordered_map>
namespace firefly::video {
class ModelSet : public core::Object, public core::Cache<ModelSet> {
private:
  std::unordered_map<std::string, core::AutoPtr<Model>> _models;
  std::unordered_map<std::string, core::AutoPtr<ModelSet>> _children;

public:
  ModelSet();
  ModelSet(const std::string &name);
  const std::unordered_map<std::string, core::AutoPtr<Model>> &
  getModels() const;
  void setModel(const std::string &name, const core::AutoPtr<Model> &model);
  const core::AutoPtr<Model> getModel(const std::string &name) const;
  const std::unordered_map<std::string, core::AutoPtr<ModelSet>> &
  getChildren() const;
  const core::AutoPtr<ModelSet> getChild(const std::string &name) const;
  void setChild(const std::string &name,
                const core::AutoPtr<ModelSet> &modelset);
};
} // namespace firefly::video