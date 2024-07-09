#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "runtime/Resource.hpp"
namespace firefly::video {
class Texture : public core::Object {
private:
  uint32_t _handle;

public:
  Texture(core::AutoPtr<runtime::Resource> source);
  ~Texture() override;
  void bind();
  void unbind();
};
}; // namespace firefly::video