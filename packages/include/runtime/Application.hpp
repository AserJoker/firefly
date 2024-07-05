#pragma once

#include "core/Object.hpp"
#include "core/EventLoop.hpp"
#include "core/Injector.hpp"
#include "Logger.hpp"
#include "ConfigProvider.hpp"
#include "CmdLine.hpp"
#include "EventBus.hpp"
#include <string>
#include <vector>
#include <filesystem>

namespace firefly::runtime {
    class Application : public core::Object {
    private:
        std::vector<std::string> _args;
        int _exitcode;
        bool _running;

    private:
        void showHelp();

    protected:
        core::Injector<core::EventLoop, "firefly.core.EventLoop"> _loop;
        core::Injector<Logger, "firefly.runtime.Logger"> _logger;
        core::Injector<CmdLine, "firefly.runtime.CmdLine"> _cmdline;
        core::Injector<ConfigProvider, "firefly.runtime.ConfigProvider"> _config;
        core::Injector<EventBus, "firefly.runtime.EventBus"> _eventbus;

    protected:
        virtual void onInitialize();

        virtual void onMainLoop();

        virtual void onUnInitialize();

    public:
        Application(int argc, char *argv[]);

        ~Application() override;

        int run();

        void exit(int nExitCode = 0);

        std::filesystem::path cwd();

    };
} // namespace firefly::runtime