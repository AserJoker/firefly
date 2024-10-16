#pragma once
#include "Promise.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "core/Promise.hpp"
#include <fstream>
namespace firefly::core {
class File : public Object {
private:
  std::fstream _handle;

public:
  File(const std::string &name, bool append = true);
  ~File() override;
  AutoPtr<Buffer> read(uint32_t size = 0);
  void write(const AutoPtr<Buffer> &data);
  Promisify<AutoPtr<Buffer>> readAsync(uint32_t size = 0);
};
} // namespace firefly::core