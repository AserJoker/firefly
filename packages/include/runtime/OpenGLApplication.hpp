#pragma once
#include "runtime/Application.hpp"
#include <SDL2/SDL.h>
namespace firefly::runtime {
class OpenGLApplication : public Application {
private:
  SDL_Window *_mainWindow;
  SDL_GLContext _glContext;

private:
  static void onMainLoop();

protected:
  void onInitialize() override;

public:
  OpenGLApplication(int argc, char *argv[]);
  ~OpenGLApplication() override;
};
} // namespace firefly::runtime