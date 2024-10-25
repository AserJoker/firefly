#pragma once
#include "Component.hpp"
#include "core/Injector.hpp"
#include "core/Point.hpp"
#include "core/Size.hpp"
#include "core/Value.hpp"
#include "document/Node.hpp"
#include "document/Window.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/ResizeEvent.hpp"
#include <glm/glm.hpp>
namespace firefly::document {
class Camera2D : public Node {
private:
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _eventBus;
  core::Point<> _position;

  glm::mat4 _view;

  Window *_window;

private:
  void applyMatrix(const core::Size<> &size);
  void onResize(runtime::ResizeEvent &);

protected:
  void onLoad() override;
  void onUnload() override;
  void onAttrChange(const core::String_t &name) override;

public:
  Camera2D();
  void setPosition(const core::Point<> &position);

public:
  constexpr static inline auto ATTR_POSITION = "position";
};
}; // namespace firefly::document