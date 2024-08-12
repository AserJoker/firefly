#pragma once
#include "Metadata.hpp"
#include "Table.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include <unordered_map>
namespace firefly::database {
class Database : public core::Object {
private:
  std::unordered_map<std::string, core::AutoPtr<Table>> _tables;

public:
  void initialize() override;
  void createTable(core::AutoPtr<Metadata> metadata);
  void deleteTable(const std::string &name);
  void updateTable(core::AutoPtr<Metadata> metadata);
  core::AutoPtr<Table> getTable(const std::string &name);
  const std::unordered_map<std::string, core::AutoPtr<Table>> &
  getTables() const;
  const core::AutoPtr<Table> getTable(const std::string &name) const;
  void load();
  void loadFromXML(const core::AutoPtr<core::Buffer> &buffer);
};

} // namespace firefly::database