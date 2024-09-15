#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
namespace firefly::core {
class File : public Object {
private:
  FILE *_handle;

public:
  File(const std::string &name, bool append = false);
  ~File() override;
  const size_t getLength() const;
  const size_t getOffset() const;
  void setOffset(size_t offset, bool absolute = true);
  core::AutoPtr<Buffer> read(size_t size = 0) const;
  void write(const core::AutoPtr<Buffer> &data);
};
} // namespace firefly::core