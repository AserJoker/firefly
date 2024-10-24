#pragma once
#include "Component.hpp"
#include "core/AutoPtr.hpp"
#include "core/Injector.hpp"
#include "document/Node.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/ResizeEvent.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>
namespace firefly::document {
class Window : public Node {
private:
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _eventBus;

  SDL_Window *_window;
  core::AutoPtr<video::Renderer> _renderer;

protected:
  virtual void onResize(runtime::ResizeEvent &);

public:
  Window();
  void onTick() override;
  const core::AutoPtr<video::Renderer> &getRenderer() const;
  core::AutoPtr<video::Renderer> &getRenderer();
};
} // namespace firefly::document