#include "video/Sprite2D.hpp"
#include "core/Singleton.hpp"
#include "video/Renderer.hpp"
using namespace firefly;
using namespace firefly::video;
Sprite2D::Sprite2D() {
  _model = glm::mat4(1.0f);
  _uv_model = glm::mat3(1.0f);
  _shader = "sprite_2d";
  _texture = "no_texture";
  _zIndex = .0f;
  _source = {0, 0, 1, 1};
  _target = {0, 0, 1, 1};
  _mesh = "rect";
}
void Sprite2D::draw() {
  auto renderer = core::Singleton<Renderer>::instance();
  renderer->bindShader(_shader);
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
const std::string &Sprite2D::getShader() const { return _shader; }
const std::string &Sprite2D::getMesh() const { return _mesh; }
void Sprite2D::setSource(const Geometry::Rect &rc) { _source = rc; }
void Sprite2D::setTarget(const Geometry::Rect &rc) { _target = rc; }
void Sprite2D::setZIndex(const float &z) { _zIndex = z; }
void Sprite2D::setTexture(const std::string &texture) { _texture = texture; }
void Sprite2D::setShader(const std::string &shader) { _shader = shader; }
void Sprite2D::setMesh(const std::string &mesh) { _mesh = mesh; }