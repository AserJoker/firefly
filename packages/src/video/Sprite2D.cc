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

void Sprite2D::update() {
  if (!_material->getTextures().contains(Material::DIFFUSE_TEX)) {
    return;
  }
  const auto &size = getTexture()->getSize();
  do {
    const auto &x = _dstRect[0];
    const auto &y = _dstRect[1];
    const auto &w = _dstRect[2];
    const auto &h = _dstRect[3];
    _matrixModel =
        glm::translate(glm::mat4(1.0f), {glm::vec2(_rotationCenter), 0}) *
        glm::rotate(glm::mat4(1.0f), _rotationAngle,
                    {0, 0, _rotationCenter.z}) *
        glm::translate(glm::mat4(1.0f), {-glm::vec2(_rotationCenter), 0.f}) *
        glm::translate(glm::mat4(1.0f), {x, y, _zIndex}) *
        glm::scale(glm::mat4(1.0f), {w, h, 1.0});
  } while (false);
  do {
    const auto &x = _srcRect[0];
    const auto &y = _srcRect[1];
    const auto &w = _srcRect[2];
    const auto &h = _srcRect[3];
    _material->getTextures().at(Material::DIFFUSE_TEX).textureCoordMatrix =
        glm::translate(glm::mat4(1.0f),
                       {x * 1.0f / size.x, y * 1.0f / size.y, 0}) *
        glm::scale(glm::mat4(1.0f),
                   {w * 1.0f / size.x, h * 1.0f / size.y, 1.0f});
  } while (false);
}

const core::AutoPtr<Geometry> &Sprite2D::getGeometry() const {
  return Geometry::get("quad");
}

const core::AutoPtr<Material> &Sprite2D::getMaterial() const {
  return _material;
}

const glm::mat4 &Sprite2D::getMatrixModel() const { return _matrixModel; }

Sprite2D::Sprite2D(const std::string &path)
    : _rotationAngle(0.0f), _matrixModel(1.0f), _zIndex(0),
      _rotationCenter(0.0f, 0.0f, 1.0f) {
  _material = new Material();
  _material->setBlend(true);
  _material->setDepthTest(false);
  _material->setBlendFunc(
      {gl::BLEND_FUNC::SRC_ALPHA, gl::BLEND_FUNC::ONE_MINUS_SRC_ALPHA});
  if (!path.empty()) {
    setTexture(path);
    auto tex = getTexture();
    auto texSize = tex->getSize();
    _dstRect = {0, 0, texSize};
    _srcRect = {0, 0, texSize};
    update();
  }
  _animation = new Animation();
  appendChild(_animation);
}

void Sprite2D::setTexture(const std::string &path) {
  _material->setTexture(Material::DIFFUSE_TEX, path);
  getTexture()->setMinifyingFilter(gl::TEXTURE_FILTER::NEAREST);
  getTexture()->setMagnificationFilter(gl::TEXTURE_FILTER::NEAREST);
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
  _dstRect = rect;
  update();
}

const glm::ivec4 &Sprite2D::getRect() const { return _dstRect; }

void Sprite2D::setSourceRect(const glm::ivec4 &rect) {
  _srcRect = rect;
  update();
}

const glm::ivec4 &Sprite2D::getSourceRect() const { return _srcRect; }

void Sprite2D::setRotation(const glm::ivec2 &center, float angle,
                           bool righthandle) {
  _rotationCenter = {center, righthandle ? -1 : 1};
  _rotationAngle = angle;
}

const std::tuple<glm::ivec2, float, bool> Sprite2D::getRotation() const {
  return {{_rotationCenter.x, _rotationCenter.y},
          _rotationAngle,
          _rotationCenter.z == -1 ? true : false};
}

void Sprite2D::setZIndex(int32_t zIndex) {
  _zIndex = zIndex;
  update();
}
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