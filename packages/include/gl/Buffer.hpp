#pragma once
#include "AccessType.hpp"
#include "BufferTraget.hpp"
#include "BufferUsage.hpp"
#include "MapAccessFlag.hpp"
#include "StorageFlag.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <initializer_list>
namespace firefly::gl {
class Buffer : public core::Object {

private:
  uint32_t _handle;
  bool _autoDeleta;

public:
  Buffer(uint32_t handle = 0);
  ~Buffer() override;

  void realloc(size_t size, void *data = 0,
               BUFFER_USAGE usage = BUFFER_USAGE::STATIC_DRAW);
  void storage(size_t size, void *data = 0,
               std::initializer_list<STORAGE_FLAG> flag = {
                   STORAGE_FLAG::MAP_READ_BIT, STORAGE_FLAG::MAP_WRITE_BIT,
                   STORAGE_FLAG::DYNAMIC_STORAGE_BIT,
                   STORAGE_FLAG::MAP_WRITE_BIT});

  void write(int64_t offset, size_t size, void *data);
  void read(int64_t offset, size_t size, void *output);
  void clear(int64_t offset, size_t size);
  void clear();

  void copy(core::AutoPtr<Buffer> target, int64_t offset, size_t size);

  void lock(size_t *size, void **output,
            ACCESS_TYPE access = ACCESS_TYPE::READ_WRITE);
  void unlock();

  void flush(int64_t offset, size_t length);

  void invalidate();
  void invalidate(int64_t offset, size_t length);

  ACCESS_TYPE getAccessType();
  MAP_ACCESS_FLAG getAccessFlags();
  bool isImmutableStorage();
  bool isMapped();
  size_t getMapLength();
  int64_t getMapOffset();
  size_t getSize();
  MAP_ACCESS_FLAG getStorageFlags();
  STORAGE_FLAG getBufferUsage();

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