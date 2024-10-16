#include "document/Sprite2D.hpp"
#include "core/AutoPtr.hpp"
#include "core/Value.hpp"
#include "document/Renderable.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Texture2D.hpp"
#include "gl/TextureFilter.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>

using namespace firefly;
using namespace firefly::document;

void Sprite2D::updateModel() {
  if (!_material->getTextures().contains(video::Material::DIFFUSE_TEX)) {
    return;
  }
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
  if (!_material->getTextures().contains(video::Material::DIFFUSE_TEX)) {
    return;
  }
  const auto &size = _material->getTextures()
                         .at(video::Material::DIFFUSE_TEX)
                         .texture->getSize();
  const auto &x = _srcRect[0];
  const auto &y = _srcRect[1];
  const auto &w = _srcRect[2];
  const auto &h = _srcRect[3];
  _material->getTextures().at(video::Material::DIFFUSE_TEX).textureCoordMatrix =
      glm::translate(glm::mat4(1.0f),
                     {x * 1.0f / size.x, y * 1.0f / size.y, 0}) *
      glm::scale(glm::mat4(1.0f), {w * 1.0f / size.x, h * 1.0f / size.y, 1.0f});
}

const core::AutoPtr<video::Geometry> &Sprite2D::getGeometry() const {
  return video::Geometry::get("quad");
}

const core::AutoPtr<video::Material> &Sprite2D::getMaterial() const {
  return _material;
}

const glm::mat4 &Sprite2D::getMatrixModel() const { return _matrixModel; }
const glm::ivec4 &Sprite2D::getBindingRect() const { return _dstRect; }

void Sprite2D::onAttrChange(const std::string &name) {
  if (name == ATTR_TEXTURE) {
    _material->setTexture(video::Material::DIFFUSE_TEX, _texture);
    auto &tex = _material->getTextures().at(video::Material::DIFFUSE_TEX);
    tex.texture->setMinifyingFilter(gl::TEXTURE_FILTER::NEAREST);
    tex.texture->setMagnificationFilter(gl::TEXTURE_FILTER::NEAREST);
    tex.blend = _blendValue;
  }
  if (name.starts_with(std::string(ATTR_RECT) + ".") ||
      name.starts_with(std::string(ATTR_ROTATION) + ".") || name == ATTR_RECT ||
      name == ATTR_ROTATION || name == ATTR_Z_INDEX) {
    updateModel();
  }
  if (name.starts_with(std::string(ATTR_SOURCE) + ".") || name == ATTR_SOURCE) {
    updateTexModel();
  }
  if (name == ATTR_VISIBLE) {
    _material->setVisible(_visible);
  }
  if (name == ATTR_BLEND) {
    _material->setBlend(_blendEnable);
    if (_material->getTextures().contains(video::Material::DIFFUSE_TEX)) {
      _material->getTextures().at(video::Material::DIFFUSE_TEX).blend =
          _blendValue;
    }
    _material->setBlendFunc({(gl::BLEND_FUNC)_blendSourceFunc,
                             (gl::BLEND_FUNC)_blendDestinationFunc});
  }
  if (name == ATTR_BLEND_ENABLE) {
    _material->setBlend(_blendEnable);
  }
  if (name == ATTR_BLEND_VALUE) {
    if (_material->getTextures().contains(video::Material::DIFFUSE_TEX)) {
      _material->getTextures().at(video::Material::DIFFUSE_TEX).blend =
          _blendValue;
    }
  }
  if (name == ATTR_BLEND_SOURCE_FUNC || name == ATTR_BLEND_DESTINATION_FUNC) {
    _material->setBlendFunc({(gl::BLEND_FUNC)_blendSourceFunc,
                             (gl::BLEND_FUNC)_blendDestinationFunc});
  }
  if (name == ATTR_ALPHA_TEST) {
    _material->setAlphaTest(_alphaTestEnable);
    _material->setAlphaFunc({(gl::ALPHA_FUNC)_alphaTestFunc, _alphaTestValue});
  }
  if (name == ATTR_ALPHA_TEST_ENABLE) {
    _material->setAlphaTest(_alphaTestEnable);
  }
  if (name == ATTR_ALPHA_TEST_FUNC || name == ATTR_ALPHA_TEST_VALUE) {
    _material->setAlphaFunc({(gl::ALPHA_FUNC)_alphaTestFunc, _alphaTestValue});
  }
  Renderable::onAttrChange(name);
}

Sprite2D::Sprite2D(const std::string &path)
    : _rotationCenter(0.0f, 0.0f, 1.0f), _rotationAngle(0.0f), _zIndex(0),
      _visible(true), _blendValue(1.0f), _matrixModel(1.0f) {
  _material = new video::Material();
  _material->setDepthTest(false);

  defineAttribute(ATTR_RECT_X, &_dstRect.x);
  defineAttribute(ATTR_RECT_Y, &_dstRect.y);
  defineAttribute(ATTR_RECT_WIDTH, &_dstRect.z);
  defineAttribute(ATTR_RECT_HEIGHT, &_dstRect.w);
  defineAttribute(ATTR_SOURCE_X, &_srcRect.x);
  defineAttribute(ATTR_SOURCE_Y, &_srcRect.y);
  defineAttribute(ATTR_SOURCE_WIDTH, &_srcRect.z);
  defineAttribute(ATTR_SOURCE_HEIGHT, &_srcRect.w);
  defineAttribute(ATTR_ROTATION_X, &_rotationCenter.x);
  defineAttribute(ATTR_ROTATION_Y, &_rotationCenter.y);
  defineAttribute(ATTR_ROTATION_ANGLE, &_rotationAngle);
  defineAttribute(ATTR_Z_INDEX, &_zIndex);
  defineAttribute(ATTR_TEXTURE, &_texture);
  defineAttribute(ATTR_VISIBLE, &_visible);
  defineAttribute(ATTR_BLEND_ENABLE, &_blendEnable);
  defineAttribute(ATTR_BLEND_VALUE, &_blendValue);
  defineAttribute(ATTR_BLEND_SOURCE_FUNC, &_blendSourceFunc);
  defineAttribute(ATTR_BLEND_DESTINATION_FUNC, &_blendDestinationFunc);
  defineAttribute(ATTR_ALPHA_TEST_ENABLE, &_alphaTestValue);
  defineAttribute(ATTR_ALPHA_TEST_FUNC, &_alphaTestFunc);
  defineAttribute(ATTR_ALPHA_TEST_VALUE, &_alphaTestValue);

  if (!path.empty()) {
    setTexture(path);
    auto tex =
        _material->getTextures().at(video::Material::DIFFUSE_TEX).texture;
    auto texSize = tex->getSize();
    setRect({0, 0, texSize});
    setRect({0, 0, texSize});
  }
  beginAttrGroup(ATTR_BLEND);
  setAttribute("enable", true);
  setAttribute("value", 1.0f);
  setAttribute("sourceFunc", (uint32_t)gl::BLEND_FUNC::SRC_ALPHA);
  setAttribute("destinationFunc",
               (uint32_t)gl::BLEND_FUNC::ONE_MINUS_SRC_ALPHA);
  endAttrGroup();

  beginAttrGroup(ATTR_ALPHA_TEST);
  setAttribute("enable", core::Value{false});
  setAttribute("func", (uint32_t)gl::ALPHA_FUNC::LESS);
  setAttribute("value", 0.0f);
  endAttrGroup();
}

void Sprite2D::setTexture(const std::string &path) {
  setAttribute(ATTR_TEXTURE, path);
}
void Sprite2D::setTexture(const core::AutoPtr<gl::Texture2D> &texture) {
  _material->setTexture(video::Material::DIFFUSE_TEX, texture);
  _texture = "";
}

const std::string &Sprite2D::getTexture() const { return _texture; }
const core::AutoPtr<gl::Texture2D> &Sprite2D::getTextureObject() const {
  return _material->getTextures().at(video::Material::DIFFUSE_TEX).texture;
}

void Sprite2D::setRect(const glm::ivec4 &rect) {
  beginAttrGroup(ATTR_RECT);
  setAttribute("x", rect.x);
  setAttribute("y", rect.y);
  setAttribute("width", rect.z);
  setAttribute("height", rect.w);
  endAttrGroup();
}

const glm::ivec4 &Sprite2D::getRect() const { return _dstRect; }

void Sprite2D::setSourceRect(const glm::ivec4 &rect) {
  beginAttrGroup(ATTR_SOURCE);
  setAttribute("x", rect.x);
  setAttribute("y", rect.y);
  setAttribute("width", rect.z);
  setAttribute("height", rect.w);
  endAttrGroup();
}

const glm::ivec4 &Sprite2D::getSourceRect() const { return _srcRect; }

void Sprite2D::setRotation(const glm::ivec2 &center, float angle) {
  beginAttrGroup(ATTR_ROTATION);
  setAttribute("x", center.x);
  setAttribute("y", center.y);
  setAttribute("angle", angle);
  endAttrGroup();
}

const std::tuple<glm::ivec2, float> Sprite2D::getRotation() const {
  return {{_rotationCenter.x, _rotationCenter.y}, _rotationAngle};
}

void Sprite2D::setZIndex(int32_t zIndex) { setAttribute(ATTR_Z_INDEX, zIndex); }

int32_t Sprite2D::getZIndex() const { return _zIndex; }

void Sprite2D::setVisible(bool visible) { setAttribute(ATTR_VISIBLE, visible); }

bool Sprite2D::isVisible() const { return _material->isVisible(); }

void Sprite2D::setBlend(float blend) { setAttribute(ATTR_BLEND_VALUE, blend); }

float Sprite2D::getBlend() const { return _blendValue; }

void Sprite2D::setBlendFunc(gl::BLEND_FUNC src, gl::BLEND_FUNC dst) {
  beginAttrGroup(ATTR_BLEND);
  setAttribute("sourceFunc", (uint32_t)src);
  setAttribute("destinationFunc", (uint32_t)dst);
  endAttrGroup();
}

const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &
Sprite2D::getBlendFunc() const {
  return _material->getBlendFunc();
}

bool Sprite2D::isAlphaTest() const { return _material->isAlphaTest(); }

void Sprite2D::setAlphaTest(bool enable) {
  setAttribute(ATTR_ALPHA_TEST_ENABLE, enable);
}

void Sprite2D::setAlphaFunc(gl::ALPHA_FUNC func, float data) {
  beginAttrGroup(ATTR_ALPHA_TEST);
  setAttribute(ATTR_ALPHA_TEST_FUNC, (uint32_t)func);
  setAttribute(ATTR_ALPHA_TEST_VALUE, data);
  endAttrGroup();
}

const std::pair<gl::ALPHA_FUNC, float> &Sprite2D::getAlphaFunc() const {
  return _material->getAlphaFunc();
}