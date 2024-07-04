#pragma once

#include "core/Object.hpp"
#include <string>
#include <vector>

namespace firefly::runtime {
    class Application : public core::Object {
    private:
        std::vector<std::string> _args;
        int _exitcode;
        bool _running;

    private:
        static void Initialize();

    protected:
        virtual void onInitialize();

    public:
        Application(int argc, char *argv[]);

        ~Application() override;

        void showHelp();

        int run();

        void exit(int nExitCode = 0);

        std::string cwd();
    };
} // namespace firefly::runtime