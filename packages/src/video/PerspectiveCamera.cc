#include "video/PerspectiveCamera.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
using namespace firefly::video;

PerspectiveCamera::PerspectiveCamera(
    const core::AutoPtr<runtime::Window> &window)
    : _window(window), _fov(45.0f), _position(0, 0, 0), _front(0, 0, 1),
      _up(0, 1.0, 0) {
  auto size = _window->getSize();
  _projection =
      glm::perspective(glm::radians(_fov), size.x / size.y, 0.1f, 1000.0f);
}
const glm::mat4 &PerspectiveCamera::getViewMatrix() { return _view; }
const glm::mat4 &PerspectiveCamera::getProjectionMatrix() {
  return _projection;
}
void PerspectiveCamera::move(float x, float y, float z) {
  _position += z * _front;
  _position += glm::normalize(glm::cross(_front, _up)) * x;
  _position += y * _up;

  _view = glm::lookAt(_position, _position + _front, _up);
}
void PerspectiveCamera::setPosition(const glm::vec3 &position) {
  _position = position;

  _view = glm::lookAt(_position, _position + _front, _up);
}
void PerspectiveCamera::rotate(float pitch, float yaw) {
  glm::vec3 front;
  front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  front.y = sin(glm::radians(pitch));
  front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  _front = glm::normalize(front);

  _view = glm::lookAt(_position, _position + _front, _up);
}
const glm::vec3 &PerspectiveCamera::getPosition() { return _position; }
void PerspectiveCamera::setFront(const glm::vec3 &front) { _front = front; }
const glm::vec3 &PerspectiveCamera::getFront() { return _front; }
void PerspectiveCamera::setUp(const glm::vec3 &up) { _up = up; }
const glm::vec3 &PerspectiveCamera::getUp() { return _up; }

void PerspectiveCamera::zoom(const float &fov) {
  _fov = fov;
  if (_fov > 45.0f) {
    _fov = 45.0f;
  }
  if (_fov < 1.0f) {
    _fov = 1.0f;
  }
  auto size = _window->getSize();
  _projection =
      glm::perspective(glm::radians(_fov), size.x / size.y, 0.1f, 1000.0f);
}
const float &PerspectiveCamera::getFov() const { return _fov; }