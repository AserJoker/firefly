#pragma once
#include "Component.hpp"
#include "core/Injector.hpp"
#include "document/Node.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_Resize.hpp"
#include <glm/glm.hpp>
namespace firefly::document {
class Scene2D : public Node {
private:
  glm::mat4 _projection;
  glm::mat4 _view;

  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _eventBus;

private:
  void onResize(runtime::Event_Resize &);

public:
  Scene2D();
  void onLoad() override;
};
}; // namespace firefly::document