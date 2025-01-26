#pragma once
#include "StaticBlock.hpp"
#include "core/typedef.hpp"
#include <unordered_map>
namespace firefly::core {
class ObjectBase {
private:
  HANDLE _handle;

private:
  static std::unordered_map<HANDLE, ObjectBase *> _table;

protected:
  static inline StaticBlock _staticBlock;

private:
  ObjectBase(const ObjectBase &) = delete;

protected:
  ObjectBase();

public:
  virtual ~ObjectBase();

  const uint32_t &ref() const;

  void *operator new(size_t size);

  void operator delete(void *opt);

  const HANDLE &getHandle() const;

public:
  static ObjectBase *select(const HANDLE &handle);
};
} // namespace firefly::core

#define STATIC_BLOCK static inline core::StaticBlock _staticBlock = []()