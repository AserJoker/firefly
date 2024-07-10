#pragma once
#include "Camera.hpp"
namespace firefly::video {
class PerspectiveCamera : public Camera {
private:
  glm::vec3 _position;
  glm::vec3 _front;
  glm::vec3 _up;

public:
  PerspectiveCamera();
  glm::mat4 getMatrix() override;
  void move(float x, float y, float z);
  void rotate(float pitch, float yaw);
  void setPosition(const glm::vec3 &position);
  const glm::vec3 &getPosition();
  void setFront(const glm::vec3 &front);
  const glm::vec3 &getFront();
  void setUp(const glm::vec3 &up);
  const glm::vec3 &getUp();
};
} // namespace firefly::video