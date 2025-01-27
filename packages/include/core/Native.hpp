#pragma once
#include "core/AutoPtr.hpp"
#include "core/ObjectBase.hpp"
namespace firefly::core {
class Native : public ObjectBase {
private:
  core::AutoPtr<ObjectBase> _native;

public:
  ~Native() override = default;
  
  void setNative(const core::AutoPtr<ObjectBase> &native);

  core::AutoPtr<ObjectBase> getNative();

  const core::AutoPtr<ObjectBase> &getNative() const;
};
}; // namespace firefly::core