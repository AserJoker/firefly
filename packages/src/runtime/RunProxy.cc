#include "runtime/RunProxy.hpp"
#include "core/Any.hpp"
#include "core/AutoPtr.hpp"
#include "core/File.hpp"
#include "core/ObjectBase.hpp"
#include "core/Singleton.hpp"
#include "runtime/Assert.hpp"
using namespace firefly;
using namespace firefly::runtime;
static core::AutoPtr<core::ObjectBase> TextLoader(const std::string &path) {
  return new core::Any(core::File::readText(path));
}
static core::AutoPtr<core::ObjectBase> BinaryLoader(const std::string &path) {
  return core::File::read(path);
}
RunProxy::RunProxy() {
  auto assert = core::Singleton<Assert>::instance();
  assert->setLoader("TEXT", TextLoader);
  assert->setLoader("BINARY", BinaryLoader);
}