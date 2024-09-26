#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Camera : public core::Object {
private:
  glm::mat4 _projection;
  glm::vec3 _position;
  glm::vec3 _front;
  glm::vec3 _up;

protected:
  glm::ivec4 _viewport;

public:
  Camera(const glm::mat4 &projection, const glm::vec3 &position,
         const glm::vec3 &front, const glm::vec3 &up);
  void setProjectionMatrix(const glm::mat4 &projection);
  void setPosition(const glm::vec3 &position);
  void setUp(const glm::vec3 &up);
  void setFront(const glm::vec3 &front);
  const glm::vec3 &getPosition() const;
  const glm::vec3 &getUp() const;
  const glm::vec3 &getFront() const;
  const glm::mat4 &getProjectionMatrix() const;
  const glm::mat4 getViewMatrix() const;
  virtual void setViewport(const glm::ivec4 &viewport);
};
} // namespace firefly::video