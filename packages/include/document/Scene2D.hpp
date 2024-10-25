#pragma once
#include "Component.hpp"
#include "core/Injector.hpp"
#include "document/Node.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/ResizeEvent.hpp"
#include <glm/glm.hpp>
namespace firefly::document {
class Scene2D : public Node {
private:
  glm::mat4 _projection;

  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _eventBus;

private:
  void onResize(runtime::ResizeEvent &);
  void onLoad() override;

public:
  Scene2D();
};
}; // namespace firefly::document