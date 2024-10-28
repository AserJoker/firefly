#include "document/Sprite2D.hpp"
#include "core/Math.hpp"
#include "core/Value.hpp"
#include "internal/geometry.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Texture.hpp"
#include <glm/ext/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::document;
Sprite2D::Sprite2D() : _zIndex(0) {
  defineProperty(PROP_TEXTURE, _texturePath);
  defineProperty(PROP_ZINDEX, _zIndex);
  defineProperty(PROP_DESTINATION, _destinationRect);
  defineProperty(PROP_SOURCE, _sourceRect);
  defineProperty(PROP_SHADER, _shader);

  watchProp(PROP_TEXTURE, &Sprite2D::onTextureChange);
  watchProp(PROP_ZINDEX, &Sprite2D::applyMatrix);
  watchProp(PROP_DESTINATION, &Sprite2D::applyMatrix);
  watchProp(PROP_SOURCE, &Sprite2D::applyTexMatrix);
  watchProp(PROP_SHADER, &Sprite2D::onShaderChange);
}

void Sprite2D::onLoad() {
  _geometry = new video::Geometry();
  _material = new video::Material();
  _matrix = glm::mat4(1.0f);

  core::AutoPtr attrPosition = new video::Attribute(internal::rectPosition, 3);
  core::AutoPtr attrCoord = new video::Attribute(internal::rectTextureCoord, 2);

  core::AutoPtr attrIndices = new video::AttributeIndex(internal::rectIndices);

  _texture = new video::Texture();
  if (!_texturePath.empty()) {
    auto [width, height] = _texture->getTexture()->getSize();
    if (_sourceRect.width == 0) {
      _sourceRect.width = width;
    }
    if (_sourceRect.height == 0) {
      _sourceRect.height = height;
    }
    if (_destinationRect.width == 0) {
      _destinationRect.width = width;
    }
    if (_destinationRect.height == 0) {
      _destinationRect.height = height;
    }
    _material->setTexture("diffuse", _texture);
  }

  _geometry->setAttribute(0, attrPosition);
  _geometry->setAttribute(1, attrCoord);
  _geometry->setAttributeIndex(attrIndices);

  _material->setIsTransparent(true);
  _material->setDepthTest(true);
  _material->setShader(_shader);

  applyTexMatrix();
  applyMatrix();
  Node::onLoad();
}

void Sprite2D::applyMatrix() {
  _matrix = core::translate({_destinationRect.x * 1.0f,
                             _destinationRect.y * 1.0f, _zIndex * 1.0f}) *
            glm::scale(glm::mat4(1.0f), {_destinationRect.width * 1.0f,
                                         _destinationRect.height * 1.0f, 1.0f});
}

void Sprite2D::applyTexMatrix() {
  if (!_texture || !_texture->getTexture()) {
    return;
  }
  auto &[width, height] = _texture->getTexture()->getSize();
  _texture->getMatrix() =
      core::scale({_sourceRect.width * 1.0f / width,
                   _sourceRect.height * 1.0f / height, 1.0f}) *
      core::translate(
          {_sourceRect.x * 1.0f / width, _sourceRect.y * 1.0f / height, 1.0f});
}

const core::AutoPtr<video::Geometry> &Sprite2D::getGeometry() const {
  return _geometry;
}

const core::AutoPtr<video::Material> &Sprite2D::getMaterial() const {
  return _material;
}

const glm::mat4 &Sprite2D::getMatrix() const { return _matrix; }

void Sprite2D::onTextureChange() {
  if (!_texture) {
    return;
  }
  _texture->setTexture(_texturePath);
  if (_material != nullptr && _material->getTexture("diffuse") != _texture) {
    _material->setTexture("diffuse", _texture);
  }
  applyTexMatrix();
}

void Sprite2D::onShaderChange() {
  if (_material != nullptr) {
    _material->setShader(_shader);
  }
}

void Sprite2D::setTexture(const core::String_t &path) {
  setProperty(PROP_TEXTURE, path);
}
const core::String_t &Sprite2D::getTexture() const { return _texturePath; }

void Sprite2D::setShader(const core::String_t &shader) {
  setProperty(PROP_SHADER, shader);
}

const core::String_t &Sprite2D::getShader() const { return _shader; }

void Sprite2D::setDestinationRect(const core::Rect<> &rect) {
  setProperty(PROP_DESTINATION, rect);
}
const core::Rect<> &Sprite2D::getDestinationRect() const {
  return _destinationRect;
}

void Sprite2D::setDestinationPosition(const core::Point<> &position) {
  setProperty(PROP_DESTINATION, position);
}
const core::Point<> &Sprite2D::getDestinationPosition() const {
  return _destinationRect.point;
}

void Sprite2D::setDestinationSize(const core::Size<> &size) {
  setProperty(PROP_DESTINATION, size);
}
const core::Size<> &Sprite2D::getDestinationSize() const {
  return _destinationRect.size;
}

void Sprite2D::setSourceRect(const core::Rect<> &rect) {
  setProperty(PROP_SOURCE, rect);
}
const core::Rect<> &Sprite2D::getSourceRect() const { return _sourceRect; }

void Sprite2D::setSourcePosition(const core::Point<> &position) {
  setProperty(PROP_SOURCE, position);
}
const core::Point<> &Sprite2D::getSourcePosition() const {
  return _sourceRect.point;
}

void Sprite2D::setSourceSize(const core::Size<> &size) {
  setProperty(PROP_SOURCE, size);
}
const core::Size<> &Sprite2D::getSourceSize() const { return _sourceRect.size; }

void Sprite2D::setZIndex(int32_t zIndex) { setProperty(PROP_ZINDEX, zIndex); }

int32_t Sprite2D::getZIndex() const { return _zIndex; }