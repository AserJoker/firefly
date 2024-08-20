#include "video/Renderer2D.hpp"
#include "core/Singleton.hpp"
#include "video/Device.hpp"
#include "video/Mesh.hpp"
#include "video/Shader.hpp"
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::video;
Renderer2D::Renderer2D() {
  _device = core::Singleton<Device>::instance();
  _mesh2D = new Mesh();
  _shader2D = new Shader();
}
void Renderer2D::draw(const core::AutoPtr<Texture> &texture,
                      const glm::vec4 &source, const glm::vec4 &target,
                      const glm::vec2 center, float angle, uint32_t zIndex) {
  glm::mat4 model(1.0);
  model = glm::scale(model, {target[2] / source[2], target[3] / source[3], 0});
  model =
      glm::translate(model, {target.x - source.x, target.y - source.y, zIndex});
  _shader2D->set("projection", _projection);
  _shader2D->set("model", model);
  _device->setTexture(texture);
  _device->setShader(_shader2D);
  _mesh2D->draw();
}

void Renderer2D::setViewport(float left, float right, float top, float bottom,
                             float near, float far) {
  _projection = glm::ortho(left, right, bottom, top, near, far);
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(0, bottom - top, 0));
  transform = glm::translate(transform, glm::vec3(0, -1, 0));
  _projection *= transform;
}