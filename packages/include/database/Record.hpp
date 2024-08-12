#pragma once
#include "Metadata.hpp"
#include "Value.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <unordered_map>
namespace firefly::database {
class Record : public core::Object {
private:
  std::unordered_map<std::string, core::AutoPtr<Value>> _data;
  core::AutoPtr<Metadata> _metadata;

public:
  Record(
      const core::AutoPtr<Metadata> &metadata,
      const std::unordered_map<std::string, core::AutoPtr<Value>> &data = {});
  core::AutoPtr<Value> getField(const std::string &name);
  const core::AutoPtr<Value> getField(const std::string &name) const;
  const core::AutoPtr<Metadata> &getMetadata() const;
  const std::string getKey() const;
  const bool hasField(const std::string &name) const;
  const bool match(const core::AutoPtr<Record> record) const;
  void merge(const core::AutoPtr<Record> record);
};
}; // namespace firefly::database