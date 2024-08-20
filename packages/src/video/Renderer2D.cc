#include "video/Renderer2D.hpp"
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::video;

void Renderer2D::draw(const core::AutoPtr<Texture> &texture,
                      const glm::vec4 &source, const glm::vec4 &target,
                      const glm::vec2 center, float angle) {
  glm::mat4 view(1.0);
  view = glm::scale(view, {target[2] / source[2], target[3] / source[3], 0});
  view = glm::translate(view, {target.x - source.x, target.y - source.y, 0});

  _shader2D->set("projection", _projection);
  _shader2D->set("view", view);
  _device->setTexture(texture);
  _device->setShader(_shader2D);
  _device->draw(GL_TRIANGLES, _mesh2D);
}

void Renderer2D::setViewport(float left, float right, float top, float bottom,
                             float near, float far) {
  _projection = glm::ortho(left, right, bottom, top, near, far);
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(0, bottom - top, 0));
  transform = glm::translate(transform, glm::vec3(0, -1, 0));
  _projection *= transform;
}