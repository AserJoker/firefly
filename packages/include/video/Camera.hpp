#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Camera : public core::Object {
private:
  glm::mat4 _projection;
  glm::mat4 _view;

protected:
  void setProjection(const glm::mat4 &projection);
  void setView(const glm::mat4 &view);

public:
  Camera();
  const glm::mat4 &getViewMatrix() const;
  const glm::mat4 &getProjectionMatrix() const;
};
} // namespace firefly::video