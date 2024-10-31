#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include <fstream>
namespace firefly::core {
class File : public Object {
private:
  std::fstream _handle;

public:
  File(const core::String_t &name, core::Boolean_t append = true);
  ~File() override;
  AutoPtr<Buffer> read(core::Unsigned_t size = 0);
  void write(const AutoPtr<Buffer> &data);
};
} // namespace firefly::core