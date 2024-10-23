#pragma once
#include "Component.hpp"
#include "core/Injector.hpp"
#include "core/Point.hpp"
#include "document/Node.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_Resize.hpp"
#include <glm/glm.hpp>
namespace firefly::document {
class Camera2D : public Node {
private:
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _eventBus;
  core::Point<> _position;
  float _zoom;

  glm::mat4 _view;

private:
  void applyMatrix(const core::Size<> &size);
  void onResize(runtime::Event_Resize &);

public:
  Camera2D();
  void onLoad() override;
  void setPosition(const core::Point<> &position);
};
}; // namespace firefly::document