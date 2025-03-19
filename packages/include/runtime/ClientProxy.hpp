#pragma once
#include "core/AutoPtr.hpp"
#include "render/Renderer.hpp"
#include "runtime/RunProxy.hpp"
#include "runtime/Window.hpp"
#include <SDL2/SDL.h>


namespace firefly::runtime {
class ClientProxy : public RunProxy {
private:
  core::AutoPtr<Window> _window;
  core::AutoPtr<render::Renderer> _renderer;

public:
  ClientProxy();
  ~ClientProxy() override;
  int run() override;
};
} // namespace firefly::runtime