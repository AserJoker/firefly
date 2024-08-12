#pragma once
#include "Metadata.hpp"
#include "Record.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
namespace firefly::database {
class Table : public core::Object {
private:
  std::vector<core::AutoPtr<Record>> _records;
  core::AutoPtr<Metadata> _metadata;
  std::unordered_map<std::string, uint32_t> _indices;

public:
  Table(const core::AutoPtr<Metadata> &metadata);
  const uint32_t getLength() const;
  const core::AutoPtr<Metadata> &getMetadata() const;
  core::AutoPtr<Record> getRecord(const uint32_t &index);
  const core::AutoPtr<Record> getRecord(const uint32_t &index) const;
  virtual core::AutoPtr<Record> queryOne(core::AutoPtr<Record> query);
  virtual std::vector<core::AutoPtr<Record>>
  queryList(core::AutoPtr<Record> query);
  virtual core::AutoPtr<Record> insertOne(core::AutoPtr<Record> query);
  virtual core::AutoPtr<Record> updateOne(core::AutoPtr<Record> query);
  virtual core::AutoPtr<Record> deleteOne(core::AutoPtr<Record> query);
  core::AutoPtr<Record> insertOrUpdateOne(core::AutoPtr<Record> query);

private:
  static std::unordered_map<std::string, std::function<core::AutoPtr<Table>(
                                             const core::AutoPtr<Metadata> &)>>
      _drivers;

public:
  template <class T> static void setDriver(const std::string &name) {
    _drivers[name] =
        [](const core::AutoPtr<Metadata> &metadata) -> core::AutoPtr<Table> {
      return new T(metadata);
    };
  }
  static core::AutoPtr<Table> create(const std::string &name,
                                     const core::AutoPtr<Metadata> &metadata);
};
} // namespace firefly::database