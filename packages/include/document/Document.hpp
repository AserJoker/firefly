#pragma once
#include "basic/Node.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::document {
class Document : public core::Object {
private:
  core::AutoPtr<Node> _root;

public:
};
} // namespace firefly::document