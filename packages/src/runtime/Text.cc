#include "runtime/Text.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Resource.hpp"
using namespace firefly::runtime;
Text::Text(core::AutoPtr<Resource> resource) {
  auto buf = resource->read();
  _source = std::string((char *)buf->getData(), buf->getSize());
}
const std::string &Text::getString() const {
  return _source;
}