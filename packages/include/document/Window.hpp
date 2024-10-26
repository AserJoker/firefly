#pragma once
#include "Component.hpp"
#include "core/AutoPtr.hpp"
#include "core/Injector.hpp"
#include "core/Point.hpp"
#include "core/Size.hpp"
#include "core/Value.hpp"
#include "document/Node.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/ResizeEvent.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>
namespace firefly::document {
class Window : public Node, public Node::Register<"window", Window> {
private:
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _eventBus;

  SDL_Window *_window;
  core::AutoPtr<video::Renderer> _renderer;

  core::String_t _title;
  core::Point<> _position;
  core::Size<> _size;
  bool _fullscreen;

protected:
  virtual void onResize(runtime::ResizeEvent &);
  void onLoad() override;
  void onUnload() override;
  void onTick() override;

public:
  Window();
  const core::AutoPtr<video::Renderer> &getRenderer() const;
  core::AutoPtr<video::Renderer> &getRenderer();
  uint32_t getWindowID() const;

public:
  constexpr static inline auto ATTR_TITLE = "title";
  constexpr static inline auto ATTR_POSITION = "position";
  constexpr static inline auto ATTR_POSITION_X = "position.x";
  constexpr static inline auto ATTR_POSITION_Y = "position.y";
  constexpr static inline auto ATTR_SIZE = "position";
  constexpr static inline auto ATTR_SIZE_X = "size.x";
  constexpr static inline auto ATTR_SIZE_Y = "size.y";
  constexpr static inline auto ATTR_FULLSCREEN = "fullscreen";
};
} // namespace firefly::document