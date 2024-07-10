#include "video/PerspectiveCamera.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
using namespace firefly::video;
PerspectiveCamera::PerspectiveCamera()
    : _position(0, 0, 0), _front(0, 0, 1), _up(0, 1.0, 0) {}
glm::mat4 PerspectiveCamera::getMatrix() {
  return glm::lookAt(_position, _position + _front, _up);
}
void PerspectiveCamera::move(float x, float y, float z) {
  _position += z * _front;
  _position += glm::normalize(glm::cross(_front, _up)) * x;
  _position += y * _up;
}
void PerspectiveCamera::setPosition(const glm::vec3 &position) {
  _position = position;
}
void PerspectiveCamera::rotate(float pitch, float yaw) {
  glm::vec3 front;
  front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  front.y = sin(glm::radians(pitch));
  front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  _front = glm::normalize(front);
}
const glm::vec3 &PerspectiveCamera::getPosition() { return _position; }
void PerspectiveCamera::setFront(const glm::vec3 &front) { _front = front; }
const glm::vec3 &PerspectiveCamera::getFront() { return _front; }
void PerspectiveCamera::setUp(const glm::vec3 &up) { _up = up; }
const glm::vec3 &PerspectiveCamera::getUp() { return _up; }