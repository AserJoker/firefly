//
// Created by w30029682 on 2024/7/5.
//
#pragma once

#include "BaseApplication.hpp"

namespace firefly::runtime {
    class Application : public BaseApplication {
    public:
        Application(int argc, char **argv);

    protected:
        void onInitialize() override;

        void onMainLoop() override;

        void onUnInitialize() override;
    };
}