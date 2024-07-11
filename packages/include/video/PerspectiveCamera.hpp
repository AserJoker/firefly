#pragma once
#include "Camera.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Window.hpp"
namespace firefly::video {
class PerspectiveCamera : public Camera {
private:
  core::AutoPtr<runtime::Window> _window;

  float _fov;

  glm::vec3 _position;
  glm::vec3 _front;
  glm::vec3 _up;

  glm::mat4 _projection;
  glm::mat4 _view;

public:
  PerspectiveCamera(const core::AutoPtr<runtime::Window> &window);
  const glm::mat4 &getViewMatrix() override;
  const glm::mat4 &getProjectionMatrix() override;
  void move(float x, float y, float z);
  void rotate(float pitch, float yaw);
  void setPosition(const glm::vec3 &position);
  const glm::vec3 &getPosition();
  void setFront(const glm::vec3 &front);
  const glm::vec3 &getFront();
  void setUp(const glm::vec3 &up);
  const glm::vec3 &getUp();
  void zoom(const float &fov);
  const float &getFov() const;
};
} // namespace firefly::video