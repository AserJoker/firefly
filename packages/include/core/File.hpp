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
  File(const std::string &name, bool append = true);
  ~File() override;
  core::AutoPtr<Buffer> read(uint32_t size = 0);
  void write(const core::AutoPtr<Buffer> &data);
};
} // namespace firefly::core