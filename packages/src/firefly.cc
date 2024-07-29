#include "firefly.hpp"
#include "Component.hpp"
#include "core/EventLoop.hpp"
#include "core/Provider.hpp"
#include "db/Database.hpp"
#include "runtime/CmdLine.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Logger.hpp"
#include "runtime/Media.hpp"
#include "script/LuaScript.hpp"

namespace firefly {
void InitFirefly() {
  auto &provider = core::Singleton<core::Provider>::instance();
  provider->provide<core::EventLoop, INJECTOR_EVENTLOOP>();
  provider->provide<runtime::Logger, INJECTOR_LOGGER>();
  provider->provide<runtime::CmdLine, INJECTOR_CMDLINE>();
  provider->provide<runtime::EventBus, INJECTOR_EVENTBUS>();
  provider->provide<runtime::Media, INJECTOR_MEDIA>();
  provider->provide<db::Database, INJECTOR_DATABASE>();
  provider->provide<script::LuaScript, INJECTOR_SCRIPT>();
}
} // namespace firefly