#pragma once
#include "Attribute.hpp"
#include "core/AutoPtr.hpp"
#include "core/Native.hpp"
namespace firefly::render {
class Geometory : public core::Native {
private:
  std::unordered_map<std::string, core::AutoPtr<Attribute>> _attributes;

public:
  Geometory() = default;
  
  ~Geometory() override = default;

  void setAttribute(const std::string &name,
                    const core::AutoPtr<Attribute> &attribute);

  const core::AutoPtr<Attribute> &getAttribute(const std::string &name) const;

  bool hasAttribute(const std::string &name) const;
};
}; // namespace firefly::render