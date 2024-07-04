#include "runtime/Application.hpp"
#include "core/Coroutine.hpp"
#include "core/EventLoop.hpp"
#include "core/Singleton.hpp"
#include "runtime/ConfigProvider.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Logger.hpp"
#include "runtime/CmdLine.hpp"
#include <filesystem>
#include <iostream>

using namespace firefly;
using namespace firefly::runtime;

void Application::Initialize() {
    auto &loop = core::Singleton<core::EventLoop>::instance();
    if (loop == nullptr) {
        loop = new core::EventLoop();
    }
    auto &bus = core::Singleton<EventBus>::instance();
    if (bus == nullptr) {
        bus = new EventBus();
    }
    auto &logger = core::Singleton<Logger>::instance();
    if (logger == nullptr) {
        logger = new Logger();
    }
    auto &config = core::Singleton<ConfigProvider>::instance();
    if (config == nullptr) {
        config = new ConfigProvider();
    }
    auto &cmdline = core::Singleton<CmdLine>::instance();
    if (cmdline == nullptr) {
        cmdline = new CmdLine();
    }
}

void Application::onInitialize() {};

Application::Application(int argc, char *argv[])
        : _args(argc), _exitcode(0), _running(true) {
    for (int index = 0; index < argc; index++) {
        _args[index] = argv[index];
    }
    Initialize();
    auto &cmdline = core::Singleton<CmdLine>::instance();
    cmdline->define("help", cmdline->placeholder, "h", "Display this information.");
}

Application::~Application() { core::CoContext::dispose(); }

int Application::run() {
    auto &cmdline = core::Singleton<CmdLine>::instance();
    auto parsed = cmdline->parse(_args);
    if (parsed.contains("help")) {
        showHelp();
        return 0;
    }
    auto theConfig = core::Singleton<ConfigProvider>::instance();
    std::filesystem::path cwd = this->cwd();
    cwd.append("config");
    theConfig->parse(cwd.string());

    onInitialize();
    auto loop = core::Singleton<core::EventLoop>::instance();
    while (_running) {
        loop->nextTick();
        core::CoContext::yield();
    }
    return _exitcode;
}

void Application::exit(int exitcode) {
    auto loop = core::Singleton<core::EventLoop>::instance();
    _exitcode = exitcode;
    _running = false;
}

std::string Application::cwd() {
    std::filesystem::path dir = _args[0];
    return dir.parent_path().string();
}

void Application::showHelp() {
    auto &cmdline = core::Singleton<CmdLine>::instance();
    std::cout << "Usage: firefly [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << cmdline->help() << std::endl;
}
