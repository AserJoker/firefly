#pragma once
#include "AccessType.hpp"
#include "BufferTraget.hpp"
#include "BufferUsage.hpp"
#include "MapAccessFlag.hpp"
#include "StorageFlag.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::gl {
class Buffer : public core::Object {

private:
  uint32_t _handle;
  bool _autoDelete;

public:
  Buffer(uint32_t handle = 0);
  ~Buffer() override;

  void setData(size_t size, void *data = 0,
               BUFFER_USAGE usage = BUFFER_USAGE::STATIC_DRAW);

  void storageData(size_t size, void *data = 0,
               uint32_t flag = STORAGE_FLAG::MAP_READ_BIT |
                               STORAGE_FLAG::MAP_WRITE_BIT |
                               STORAGE_FLAG::DYNAMIC_STORAGE_BIT |
                               STORAGE_FLAG::MAP_WRITE_BIT);

  void write(int64_t offset, size_t size, void *data);

  void read(int64_t offset, size_t size, void *output);

  void copy(core::AutoPtr<Buffer> target, int64_t srcOffset, int64_t offset,
            size_t size);

  void map(size_t *size, void **output,
           ACCESS_TYPE access = ACCESS_TYPE::READ_WRITE);

  void mapRange(size_t *size, void **output, int64_t offset, size_t length,
                uint32_t access = MAP_ACCESS_FLAG::READ_BIT |
                                  MAP_ACCESS_FLAG::WRITE_BIT);
  void unmap();

  void flush(int64_t offset, size_t length);

  void invalidate();

  void invalidate(int64_t offset, size_t length);

  ACCESS_TYPE getAccessType();

  uint32_t getAccessFlags();

  bool isImmutableStorage();

  bool isMapped();

  size_t getMapLength();

  int64_t getMapOffset();

  size_t getSize();

  uint32_t getStorageFlags();

  BUFFER_USAGE getUsage();

  void *getMappedPointer();

  uint32_t getHandle();

public:
  static void bind(BUFFER_TARGET target, core::AutoPtr<Buffer> buffer);

  static void unbind(BUFFER_TARGET target);

  static void bindRange(BUFFER_TARGET target, uint32_t index,
                        core::AutoPtr<Buffer> buffer, uint32_t offset,
                        uint32_t size);

  static void bindBase(BUFFER_TARGET target, uint32_t index,
                       core::AutoPtr<Buffer> buffer);
};
}; // namespace firefly::gl