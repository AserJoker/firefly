#include "video/Camera.hpp"
using namespace firefly;
using namespace firefly::video;

Camera::Camera() : _projection(1.0f), _view(1.0f){};
const glm::mat4 &Camera::getViewMatrix() const { return _view; }
const glm::mat4 &Camera::getProjectionMatrix() const { return _projection; }
void Camera::setProjection(const glm::mat4 &projection) {
  _projection = projection;
}
void Camera::setView(const glm::mat4 &view) { _view = view; }