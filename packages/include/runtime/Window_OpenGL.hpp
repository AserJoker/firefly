#pragma once
#include "IWindow.hpp"
#include "runtime/IWindow.hpp"
#include <SDL2/SDL.h>
namespace firefly::runtime {
class Window_OpenGL : public IWindow {
private:
  SDL_Window *_window;
  SDL_GLContext _ctx;

public:
  Window_OpenGL(const std::string &title, int width, int height);
  void setTitle(const std::string &title) override;
  std::string getTitle() const override;
  void present() override;
  core::AutoPtr<video::IRenderer> createRenderer() override;
};
} // namespace firefly::runtime