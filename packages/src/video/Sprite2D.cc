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
  _textureRect = {0, 0, size.w, size.h};
  _rect = {0, 0, size.w * 50, size.h * 50};
  _rotation.angle = 0;
  _rotation.center = {0, 0, 1};
  _mesh = "rect";
  updateModel();
  updateUVModel();
}

void Sprite2D::updateModel() {
  glm::mat4 m = glm::mat4(1.0f);
  auto dx = _rotation.center.x - _rect.position.x;
  auto dy = _rotation.center.y - _rect.position.y;
  float nz = _rotation.center.z / abs(_rotation.center.z);
  m *= glm::transpose(glm::translate(glm::mat4(1.0f), glm::vec3(-dx, -dy, 0)));
  m *=
      glm::transpose(glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.angle),
                                 glm::vec3(0, 0, _rotation.center.z)));
  m *= glm::transpose(glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0)));
  _model =
      glm::scale(glm::mat4(1.0f), glm::vec3(_rect.size.w, _rect.size.h, 1.0f));
  _model = glm::transpose(glm::translate(
               glm::mat4(1.0f),
               glm::vec3(_rect.position.x, _rect.position.y, _zIndex))) *
           glm::transpose(m) * _model;
}
void Sprite2D::updateUVModel() {
  auto renderer = core::Singleton<Renderer>::instance();
  auto tex = renderer->getTexture(_texture).cast<Texture2D>();
  auto size = tex->getTextureSize();
  _uv_model = glm::mat4(1.0f);
  _uv_model =
      glm::translate(_uv_model, glm::vec3(_textureRect.position.x / size.w,
                                          _textureRect.position.y / size.h, 0));
  _uv_model =
      glm::scale(_uv_model, glm::vec3(_textureRect.size.w / size.w,
                                      _textureRect.size.h / size.h, 1.0f));
}
void Sprite2D::draw() {
  auto renderer = core::Singleton<Renderer>::instance();
  auto shader = renderer->getCurrentShader();
  renderer->bindTexture(_texture, 0);

  shader->setUniform("uv_model", _uv_model);
  shader->setUniform("model", _model);
  renderer->drawMesh(_mesh);
}
const Geometry::Rect &Sprite2D::getTextureRect() const { return _textureRect; }
const Geometry::Rect &Sprite2D::getRect() const { return _rect; }
const float &Sprite2D::getZIndex() const { return _zIndex; }
const std::string &Sprite2D::getTexture() const { return _texture; }
const std::string &Sprite2D::getMesh() const { return _mesh; }

const Geometry::Rotation &Sprite2D::getRotation() const { return _rotation; }
void Sprite2D::setTextureRect(const Geometry::Rect &rc) {
  _textureRect = rc;
  updateUVModel();
}
void Sprite2D::setRect(const Geometry::Rect &rc) {
  _rect = rc;
  updateModel();
}
void Sprite2D::setZIndex(const float &z) {
  _zIndex = z;
  updateModel();
}
void Sprite2D::setTexture(const std::string &texture) {
  _texture = texture;
  auto renderer = core::Singleton<Renderer>::instance();
  auto tex = renderer->getTexture(_texture).cast<Texture2D>();
  _rect.size = tex->getTextureSize();
  _textureRect.size = tex->getTextureSize();
  updateModel();
  updateUVModel();
}
void Sprite2D::setMesh(const std::string &mesh) { _mesh = mesh; }
void Sprite2D::setRotation(const Geometry::Rotation &rot) {
  _rotation = rot;
  updateModel();
}