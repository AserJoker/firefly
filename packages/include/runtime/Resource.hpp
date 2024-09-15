#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
namespace firefly::runtime {
class Resource : public core::Object {
public:
  virtual core::AutoPtr<core::Buffer> read(uint32_t size = 0) = 0;
  virtual void write(const core::AutoPtr<core::Buffer> &data) = 0;
};
template <class T> class ResourceTrait : public Resource {
private:
  core::AutoPtr<T> _handle;

public:
  template <class... ARGS> ResourceTrait(ARGS &&...args) {
    _handle = new T(args...);
  }
  core::AutoPtr<core::Buffer> read(uint32_t size = 0) override {
    return _handle->read(size);
  };
  void write(const core::AutoPtr<core::Buffer> &data) override {
    _handle->write(data);
  };
};
} // namespace firefly::runtime