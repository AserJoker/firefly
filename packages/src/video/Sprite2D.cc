#include "video/Sprite2D.hpp"
#include "core/Singleton.hpp"
#include "video/Geometry.hpp"
#include "video/Renderer.hpp"
#include "video/Texture2D.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
using namespace firefly;
using namespace firefly::video;
Sprite2D::Sprite2D() {
  Geometry::Size size = {2, 2};
  _texture = "no_texture";
  _zIndex = .0f;
  _source = {0, 0, size.w, size.h};
  _target = {0, 0, size.w * 50, size.h * 50};
  _rotation.angle = 0;
  _rotation.center = {0, 0, 1};
  _mesh = "rect";
  updateModel();
  updateUVModel();
}

void Sprite2D::updateModel() {
  glm::mat4 m = glm::mat4(1.0f);
  m *= glm::mat4({1, 0, 0, -_rotation.center.x, 0, 1, 0, -_rotation.center.y, 0,
                  0, 1, 0, 0, 0, 0, 1});
  float nz = _rotation.center.z / abs(_rotation.center.z);
  m *= glm::mat4({cos(_rotation.angle), -sin(_rotation.angle) * nz, 0, 0,
                  sin(_rotation.angle) * nz, cos(_rotation.angle), 0, 0, 0, 0,
                  1, 0, 0, 0, 0, 1});

  m *= glm::mat4({1, 0, 0, _rotation.center.x, 0, 1, 0, _rotation.center.y, 0,
                  0, 1, 0, 0, 0, 0, 1});

  _model = glm::mat4(1.0f);
  _model = glm::scale(_model, glm::vec3(_target.size.w, _target.size.h, 1.0f));
  _model = glm::translate(
      _model, glm::vec3(_target.position.x, _target.position.y, _zIndex));
  _model = glm::transpose(m) * _model;
}
void Sprite2D::updateUVModel() {
  auto renderer = core::Singleton<Renderer>::instance();
  auto tex = renderer->getTexture(_texture).cast<Texture2D>();
  auto size = tex->getTextureSize();
  _uv_model = glm::mat4(1.0f);
  _uv_model =
      glm::translate(_uv_model, glm::vec3(_source.position.x / size.w,
                                          _source.position.y / size.h, 0));
  _uv_model = glm::scale(_uv_model, glm::vec3(_source.size.w / size.w,
                                              _source.size.h / size.h, 1.0f));
}
void Sprite2D::draw() {
  auto renderer = core::Singleton<Renderer>::instance();
  auto shader = renderer->getCurrentShader();
  renderer->bindTexture(_texture, 0);

  shader->setUniform("uv_model", _uv_model);
  shader->setUniform("model", _model);
  renderer->drawMesh(_mesh);
}
const Geometry::Rect &Sprite2D::getSource() const { return _source; }
const Geometry::Rect &Sprite2D::getTarget() const { return _target; }
const float &Sprite2D::getZIndex() const { return _zIndex; }
const std::string &Sprite2D::getTexture() const { return _texture; }
const std::string &Sprite2D::getMesh() const { return _mesh; }

const Geometry::Rotation &Sprite2D::getRotation() const { return _rotation; }
void Sprite2D::setSource(const Geometry::Rect &rc) {
  _source = rc;
  updateUVModel();
}
void Sprite2D::setTarget(const Geometry::Rect &rc) {
  _target = rc;
  updateModel();
}
void Sprite2D::setZIndex(const float &z) {
  _zIndex = z;
  updateModel();
}
void Sprite2D::setTexture(const std::string &texture) {
  _texture = texture;
  updateUVModel();
}
void Sprite2D::setMesh(const std::string &mesh) { _mesh = mesh; }
void Sprite2D::setRotation(const Geometry::Rotation &rot) {
  _rotation = rot;
  updateModel();
}