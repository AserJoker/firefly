#include "video/Sprite2D.hpp"
#include "core/AutoPtr.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Texture2D.hpp"
#include "gl/TextureFilter.hpp"
#include "video/Animation.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include <glm/ext/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::video;

void Sprite2D::updateModel() {
  if (!_material->getTextures().contains(Material::DIFFUSE_TEX)) {
    return;
  }
  const auto &size = getTexture()->getSize();
  const auto &x = _dstRect[0];
  const auto &y = _dstRect[1];
  const auto &w = _dstRect[2];
  const auto &h = _dstRect[3];
  _matrixModel =
      glm::translate(glm::mat4(1.0f), {glm::vec2(_rotationCenter), 0}) *
      glm::rotate(glm::mat4(1.0f), _rotationAngle, {0, 0, _rotationCenter.z}) *
      glm::translate(glm::mat4(1.0f), {-glm::vec2(_rotationCenter), 0.f}) *
      glm::translate(glm::mat4(1.0f), {x, y, _zIndex}) *
      glm::scale(glm::mat4(1.0f), {w, h, 1.0});
}
void Sprite2D::updateTexModel() {
  if (!_material->getTextures().contains(Material::DIFFUSE_TEX)) {
    return;
  }
  const auto &size = getTexture()->getSize();
  const auto &x = _srcRect[0];
  const auto &y = _srcRect[1];
  const auto &w = _srcRect[2];
  const auto &h = _srcRect[3];
  _material->getTextures().at(Material::DIFFUSE_TEX).textureCoordMatrix =
      glm::translate(glm::mat4(1.0f),
                     {x * 1.0f / size.x, y * 1.0f / size.y, 0}) *
      glm::scale(glm::mat4(1.0f), {w * 1.0f / size.x, h * 1.0f / size.y, 1.0f});
}

const core::AutoPtr<Geometry> &Sprite2D::getGeometry() const {
  return Geometry::get("quad");
}

const core::AutoPtr<Material> &Sprite2D::getMaterial() const {
  return _material;
}

const glm::mat4 &Sprite2D::getMatrixModel() const { return _matrixModel; }

void Sprite2D::onAttrChange(const std::string &name) {
  if (name == "texture") {
    _material->setTexture(Material::DIFFUSE_TEX, _texture);
    getTexture()->setMinifyingFilter(gl::TEXTURE_FILTER::NEAREST);
    getTexture()->setMagnificationFilter(gl::TEXTURE_FILTER::NEAREST);
  }
  if (name.starts_with("rect.") || name.starts_with("rotation.") ||
      name == "rect" || name == "rotation" || name == "zIndex") {
    updateModel();
  }
  if (name.starts_with("source.") || name == "source") {
    updateTexModel();
  }
}

Sprite2D::Sprite2D(const std::string &path)
    : _rotationAngle(0.0f), _matrixModel(1.0f), _zIndex(0),
      _rotationCenter(0.0f, 0.0f, 1.0f) {
  _material = new Material();
  _animation = new Animation();

  _material->setBlend(true);
  _material->setDepthTest(false);
  _material->setBlendFunc(
      {gl::BLEND_FUNC::SRC_ALPHA, gl::BLEND_FUNC::ONE_MINUS_SRC_ALPHA});

  defineAttribute("rect.x", &_dstRect.x);
  defineAttribute("rect.y", &_dstRect.y);
  defineAttribute("rect.width", &_dstRect.z);
  defineAttribute("rect.height", &_dstRect.w);
  defineAttribute("source.x", &_srcRect.x);
  defineAttribute("source.y", &_srcRect.y);
  defineAttribute("source.width", &_srcRect.z);
  defineAttribute("source.height", &_srcRect.w);
  defineAttribute("rotation.x", &_rotationCenter.x);
  defineAttribute("rotation.y", &_rotationCenter.y);
  defineAttribute("rotation.angle", &_rotationAngle);
  defineAttribute("zIndex", &_zIndex);
  defineAttribute("texture", &_texture);
  if (!path.empty()) {
    setTexture(path);
    auto tex = getTexture();
    auto texSize = tex->getSize();
    setRect({0, 0, texSize});
    setRect({0, 0, texSize});
  }
  appendChild(_animation);
  if (_texture == "001-Fighter01.png") {
    _animation->setFPS(10);
    _animation->createAction("move", "source.x", 0, 3 * 32, 0, 3, true);
    _animation->start("move");
  }
}

void Sprite2D::setTexture(const std::string &path) {
  setAttribute("texture", path);
}
void Sprite2D::setTexture(const core::AutoPtr<gl::Texture2D> &texture) {
  _material->setTexture(Material::DIFFUSE_TEX, texture);
}

core::AutoPtr<gl::Texture2D> &Sprite2D::getTexture() {
  return _material->getTextures().at(Material::DIFFUSE_TEX).texture;
}

const core::AutoPtr<gl::Texture2D> &Sprite2D::getTexture() const {
  return _material->getTextures().at(Material::DIFFUSE_TEX).texture;
}

void Sprite2D::setRect(const glm::ivec4 &rect) {
  beginAttrGroup("rect");
  setAttribute("x", rect.x);
  setAttribute("y", rect.y);
  setAttribute("width", rect.z);
  setAttribute("height", rect.w);
  endAttrGroup();
}

const glm::ivec4 &Sprite2D::getRect() const { return _dstRect; }

void Sprite2D::setSourceRect(const glm::ivec4 &rect) {
  beginAttrGroup("source");
  setAttribute("x", rect.x);
  setAttribute("y", rect.y);
  setAttribute("width", rect.z);
  setAttribute("height", rect.w);
  endAttrGroup();
}

const glm::ivec4 &Sprite2D::getSourceRect() const { return _srcRect; }

void Sprite2D::setRotation(const glm::ivec2 &center, float angle) {
  beginAttrGroup("rotation");
  setAttribute("x", center.x);
  setAttribute("y", center.y);
  setAttribute("angle", angle);
  endAttrGroup();
}

const std::tuple<glm::ivec2, float> Sprite2D::getRotation() const {
  return {{_rotationCenter.x, _rotationCenter.y}, _rotationAngle};
}

void Sprite2D::setZIndex(int32_t zIndex) { setAttribute("zIndex", zIndex); }

int32_t Sprite2D::getZIndex() const { return _zIndex; }

void Sprite2D::setVisible(bool visible) { _material->setVisible(visible); }

bool Sprite2D::isVisible() const { return _material->isVisible(); }

void Sprite2D::setBlend(float blend) {
  _material->getTextures().at(Material::DIFFUSE_TEX).blend = blend;
}

float Sprite2D::getBlend() const {
  return _material->getTextures().at(Material::DIFFUSE_TEX).blend;
}

void Sprite2D::setBlendFunc(gl::BLEND_FUNC dst, gl::BLEND_FUNC src) {
  _material->setBlendFunc({dst, src});
}

const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &
Sprite2D::getBlendFunc() const {
  return _material->getBlendFunc();
}

bool Sprite2D::isAlphaTest() const { return _material->isAlphaTest(); }

void Sprite2D::setAlphaTest(bool enable) { _material->setAlphaTest(enable); }

void Sprite2D::setAlphaFunc(gl::ALPHA_FUNC func, float data) {
  _material->setAlphaFunc({func, data});
}

const std::pair<gl::ALPHA_FUNC, float> &Sprite2D::getAlphaFunc() const {
  return _material->getAlphaFunc();
}