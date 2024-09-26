#include "video/Camera.hpp"
#include <glm/ext/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::video;
Camera::Camera(const glm::mat4 &projection, const glm::vec3 &position,
               const glm::vec3 &front, const glm::vec3 &up)
    : _projection(projection), _position(position), _front(front), _up(up) {}

void Camera::setProjectionMatrix(const glm::mat4 &projection) {
  _projection = projection;
};
void Camera::setPosition(const glm::vec3 &position) { _position = position; }
void Camera::setUp(const glm::vec3 &up) { _up = up; }
void Camera::setFront(const glm::vec3 &front) { _front = front; }
const glm::vec3 &Camera::getPosition() const { return _position; }
const glm::vec3 &Camera::getUp() const { return _up; }
const glm::vec3 &Camera::getFront() const { return _front; }
const glm::mat4 &Camera::getProjectionMatrix() const { return _projection; }
const glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(_position, _position + _front, _up);
}
void Camera::setViewport(const glm::ivec4 &viewport) { _viewport = viewport; }
