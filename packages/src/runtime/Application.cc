//
// Created by w30029682 on 2024/7/5.
//
#include "runtime/Application.hpp"

using namespace firefly::runtime;

Application::Application(int argc, char **argv) : BaseApplication(argc, argv) {

}

void Application::onInitialize() {
    BaseApplication::onInitialize();
}

void Application::onMainLoop() {
    BaseApplication::onMainLoop();
    exit();
}

void Application::onUnInitialize() {
    BaseApplication::onUnInitialize();
}
