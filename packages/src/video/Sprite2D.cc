#include "video/Sprite2D.hpp"
#include "gl/TextureFilter.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include <glm/ext/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::video;
Sprite2D::Sprite2D(const std::string &path)
    : _rotationAngle(0.0f), _matrixModel(1.0f) {
  _material = new Material();
  setTexture(path);
  auto tex = getTexture();
  auto texSize = tex->getSize();
  _dstRect = {0, 0, texSize};
  _srcRect = {0, 0, texSize};
  _matrixModel = glm::scale(glm::mat4(1.0f), {texSize, 1.0f});
  _material->setBlend(true);
  _material->setDepthTest(true);
  _material->getTextures().at(Material::DIFFUSE_TEX).textureCoordMatrix =
      glm::scale(glm::mat4(1.0f), {texSize, 1.0f});
}
void Sprite2D::setTexture(const std::string &path) {
  _material->setTexture(Material::DIFFUSE_TEX, path);
  getTexture()->setMinifyingFilter(gl::TEXTURE_FILTER::NEAREST);
  getTexture()->setMagnificationFilter(gl::TEXTURE_FILTER::NEAREST);
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
void Sprite2D::setSourceRect(const glm::ivec4 &rect) {
  _srcRect = rect;
  update();
}
void Sprite2D::setRotation(const glm::ivec2 &center, float angle,
                           bool righthandle) {
  _rotationCenter = {center, righthandle ? -1 : 1};
  _rotationAngle = angle;
}
void Sprite2D::update() {
  const auto &size = getTexture()->getSize();
  do {
    const auto &x = _dstRect[0];
    const auto &y = _dstRect[1];
    const auto &w = _dstRect[2];
    const auto &h = _dstRect[3];
    _matrixModel = glm::translate(glm::mat4(1.0f), {x * 1.0f, y * 1.0f, 0}) *
                   glm::translate(glm::mat4(1.0f),
                                  {_rotationCenter.x, _rotationCenter.y, 0.f}) *
                   glm::rotate(glm::mat4(1.0f), _rotationAngle,
                               {0, 0, _rotationCenter.z}) *
                   glm::translate(glm::mat4(1.0f), {-_rotationCenter.x,
                                                    -_rotationCenter.y, 0.f}) *
                   glm::scale(glm::mat4(1.0f), {w, h, 1.0f});
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
const glm::mat4 &Sprite2D::getModelMatrix() const { return _matrixModel; }