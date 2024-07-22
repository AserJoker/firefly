#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "db/Table.hpp"
#include <string>
#include <unordered_map>
namespace firefly::db {
class Database : public core::Object {
public:
  struct Schema {
    std::unordered_map<std::string, Schema> children;
    std::string type;
  };

private:
  std::unordered_map<std::string, core::AutoPtr<Table>> _tables;

private:
  std::unordered_map<std::string, std::any>
  updateComplexField(const std::string &table,
                     const std::unordered_map<std::string, std::any> &record);
  std::unordered_map<std::string, std::any>
  resolveSchema(const Schema &schema, const core::AutoPtr<Table> &driver,
                const core::AutoPtr<Record> &record);

public:
  void initialize() override;
  core::AutoPtr<Table> getTable(const std::string &table);
  core::AutoPtr<Table> createTable(const std::string &driver,
                                   const std::string &name,
                                   const std::string &ns,
                                   const std::vector<Field> &fields,
                                   const std::vector<std::string> &primaryKeys);
  core::AutoPtr<Table> createTable(const core::AutoPtr<Record> &meta);
  core::AutoPtr<Table> updateTable(const core::AutoPtr<Record> &meta);
  core::AutoPtr<Table> deleteTable(const std::string &id);
  core::AutoPtr<Record>
  insertOne(const std::string &table,
            const std::unordered_map<std::string, std::any> &record);
  core::AutoPtr<Record>
  updateOne(const std::string &table,
            const std::unordered_map<std::string, std::any> &record);
  core::AutoPtr<Record>
  deleteOne(const std::string &table,
            const std::unordered_map<std::string, std::any> &record);
  core::AutoPtr<Record>
  insertOrUpdateOne(const std::string &table,
                    const std::unordered_map<std::string, std::any> &record);
  core::AutoPtr<Record>
  queryOne(const std::string &table,
           const std::unordered_map<std::string, std::any> &filter = {},
           const Schema &schema = {});
  std::vector<core::AutoPtr<Record>>
  queryList(const std::string &table,
            const std::unordered_map<std::string, std::any> &filter = {},
            const Schema &schema = {});
};
#define DB_DEFAULT_DRIVER "firefly.db.table"
#define DB_TABLE_DRIVER "firefly.db.table_table"
}; // namespace firefly::db