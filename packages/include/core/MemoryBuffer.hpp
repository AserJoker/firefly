#pragma once
#include "Buffer.hpp"
#include "Readable.hpp"
#include "Writable.hpp"
#include "core/AutoPtr.hpp"
#include "core/ObjectBase.hpp"
namespace firefly::core {
class MemoryBuffer : public ObjectBase,
                     public Buffer,
                     public Readable,
                     public Writable {
private:
  void *_buf;
  size_t _size;

public:
  MemoryBuffer(size_t size = 0, const void *source = nullptr);

  MemoryBuffer(const core::AutoPtr<Buffer> &buffer);

  ~MemoryBuffer() override;

  const void *getRaw() const override;

  size_t getSize() const override;

  void resize(size_t size);

  const core::AutoPtr<Buffer> read(size_t offset = 0,
                                   size_t size = 0) const override;

  void write(const core::AutoConstPtr<Buffer> &buffer,
             size_t offset = 0) override;

  bool isReadable() const override;
  bool isWritable() const override;
};
} // namespace firefly::core