#include "firefly.hpp"
#include "Component.hpp"
#include "core/Provider.hpp"
#include "runtime/CmdLine.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Locale.hpp"
#include "runtime/Logger.hpp"
#include "runtime/Media.hpp"
#include "runtime/ModLoader.hpp"
#include "script/Script.hpp"
#include "video/Renderer.hpp"

namespace firefly {
void InitFirefly() {
  auto &provider = core::Singleton<core::Provider>::instance();
  provider->provide<runtime::Logger, INJECTOR_LOGGER>();
  provider->provide<runtime::CmdLine, INJECTOR_CMDLINE>();
  provider->provide<runtime::EventBus, INJECTOR_EVENTBUS>();
  provider->provide<runtime::Media, INJECTOR_MEDIA>();
  provider->provide<runtime::Locale, INJECTOR_LOCALE>();
  provider->provide<runtime::ModLoader, INJECTOR_MOD>();
  provider->provide<script::Script, INJECTOR_SCRIPT>();
  provider->provide<video::Renderer, INJECTOR_RENDERER>();
}
} // namespace firefly