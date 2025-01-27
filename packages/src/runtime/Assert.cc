#include "runtime/Assert.hpp"
#include "core/AutoPtr.hpp"
#include "core/Exception.hpp"
#include "core/ObjectBase.hpp"
using namespace firefly;
using namespace firefly::runtime;
core::AutoPtr<core::ObjectBase> Assert::load(const std::string &type,
                                             const std::string &path) const {
  if (!_loader.contains(type)) {
    throw core::Exception(fmt::format("Unknown assert type: '{}'", type));
  }
  return _loader.at(type)(path);
}