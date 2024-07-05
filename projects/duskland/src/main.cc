#include <SDL2/SDL.h>
#include "core/Singleton.hpp"
#include "runtime/BaseApplication.hpp"
#include "runtime/Logger.hpp"
#include "core/Provider.hpp"
#include "core/EventLoop.hpp"
#include "runtime/Application.hpp"
#include <exception>

using namespace firefly;

int main(int argc, char *argv[]) {
    auto &provider = core::Singleton<core::Provider>::instance();

    provider->provide<core::EventLoop, "firefly.core.EventLoop">();
    provider->provide<runtime::Logger, "firefly.runtime.Logger">();
    provider->provide<runtime::CmdLine, "firefly.runtime.CmdLine">();
    provider->provide<runtime::EventBus, "firefly.runtime.EventBus">();
    provider->provide<runtime::ConfigProvider, "firefly.runtime.ConfigProvider">();

    try {
        core::Singleton<runtime::Application>::initialize<runtime::Application>(argc, argv);
        return core::Singleton<runtime::Application>::instance()->run();
    } catch (std::exception &e) {
        auto &theLogger = core::Singleton<runtime::Logger>::instance();
        theLogger->panic("{}", e.what());
    }
    return 0;
}