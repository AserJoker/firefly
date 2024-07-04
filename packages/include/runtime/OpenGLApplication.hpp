#pragma once

#include "runtime/Application.hpp"
#include <SDL2/SDL.h>

namespace firefly::runtime {
    class OpenGLApplication : public Application {
    private:
        SDL_Window *_mainWindow;
        SDL_GLContext _glContext;
        struct {
            std::string engine;
        } _config;

    private:
        void initOpenGL();

    protected:
        void onInitialize() override;

        virtual void onMainLoop();

    public:
        OpenGLApplication(int argc, char *argv[]);

        ~OpenGLApplication() override;
    };
} // namespace firefly::runtime