#pragma once
#include "core/AutoPtr.hpp"
#include "database/Metadata.hpp"
#include "database/Table.hpp"
namespace firefly::database {
class Driver_Table : public Table {
private:
  core::AutoPtr<Metadata> createMetadata(core::AutoPtr<Record> record);
  Field createField(core::AutoPtr<Record> record);

public:
  Driver_Table(const core::AutoPtr<Metadata>& metadata);
  core::AutoPtr<Record> insertOne(core::AutoPtr<Record> query) override;
  core::AutoPtr<Record> updateOne(core::AutoPtr<Record> query) override;
  core::AutoPtr<Record> deleteOne(core::AutoPtr<Record> query) override;
  constexpr static inline auto DRIVER_NAME = "firefly.driver.table";
};
} // namespace firefly::database