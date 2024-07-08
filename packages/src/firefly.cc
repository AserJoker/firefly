#include "firefly.hpp"
#include "core/EventLoop.hpp"
#include "core/Provider.hpp"
#include "injector.hpp"
#include "runtime/CmdLine.hpp"
#include "runtime/ConfigProvider.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Logger.hpp"

namespace firefly {
void InitFirefly() {
  auto &provider = core::Singleton<core::Provider>::instance();
  provider->provide<core::EventLoop, INJECTOR_EVENTLOOP>();
  provider->provide<runtime::Logger, INJECTOR_LOGGER>();
  provider->provide<runtime::CmdLine, INJECTOR_CMDLINE>();
  provider->provide<runtime::EventBus, INJECTOR_EVENTBUS>();
  provider->provide<runtime::ConfigProvider, INJECTOR_CONFIGPROVIDER>();
}
} // namespace firefly