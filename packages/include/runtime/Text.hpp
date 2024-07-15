#pragma once
#include "Resource.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::runtime {
class Text : public core::Object {
private:
  std::string _source;

public:
  Text(core::AutoPtr<Resource> resource);
  const std::string &getString() const;
};
} // namespace firefly::runtime