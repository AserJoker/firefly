#pragma once
#include "Entity.hpp"
#include "Field.hpp"
#include "Record.hpp"
#include "core/AutoPtr.hpp"
#include <any>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::db {
class Table : public Entity {

private:
  std::vector<std::string> _primaryKeys;
  std::vector<Field> _fields;
  std::unordered_map<std::string, uint32_t> _fieldIndices;
  std::unordered_map<std::string, core::AutoPtr<Record>> _records;

private:
  std::string
  getRecordKey(const std::unordered_map<std::string, std::any> &record);

public:
  Table() = default;
  void Initialize(const std::string &name, const std::string &ns,
                  const std::vector<Field> &fields,
                  const std::vector<std::string> &primaryKeys = {"id"});
  void Initialize(const core::AutoPtr<Record> &table);
  const std::vector<Field> &getFields() const;
  const Field &getField(const std::string &name) const;
  const bool hasField(const std::string &name) const;
  const std::vector<std::string> &getPrimaryKeys() const;
  virtual const core::AutoPtr<Record>
  queryOne(const std::unordered_map<std::string, std::any> &filter) const;
  virtual std::vector<core::AutoPtr<Record>>
  queryList(const std::unordered_map<std::string, std::any> &filter = {}) const;
  virtual const core::AutoPtr<Record>
  insertOne(const std::unordered_map<std::string, std::any> &record);
  virtual const core::AutoPtr<Record>
  updateOne(const std::unordered_map<std::string, std::any> &record);
  virtual const core::AutoPtr<Record>
  insertOrUpdateOne(const std::unordered_map<std::string, std::any> &record);
  virtual const core::AutoPtr<Record>
  deleteOne(const std::unordered_map<std::string, std::any> &record);
};
}; // namespace firefly::db