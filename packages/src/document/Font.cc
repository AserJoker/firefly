#include "document/Font.hpp"
#include "core/AutoPtr.hpp"
#include "core/Color.hpp"
#include "core/Math.hpp"
#include "internal/geometry.hpp"
#include "video/Attribute.hpp"
#include "video/Font.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Texture.hpp"
#include <codecvt>
#include <locale>

using namespace firefly;
using namespace firefly::document;
Font::Font() : _zIndex(0), _matrix(1.0f) {
  _shader = "font";
  _source = "simhei.ttf";

  defineProperty(PROP_SOURCE, _source);
  defineProperty(PROP_SHADER, _shader);
  defineProperty(PROP_ZINDEX, _zIndex);

  watchProp(PROP_SOURCE, &Font::onSourceChange);
  watchProp(PROP_SHADER, &Font::onShaderChange);
  watchProp(PROP_ZINDEX, &Font::update);
}

const core::AutoPtr<video::Geometry> &Font::getGeometry() const {
  return _geometry;
};

const core::AutoPtr<video::Material> &Font::getMaterial() const {
  return _material;
};

const glm::mat4 &Font::getMatrix() const { return _matrix; };

void Font::onLoad() {
  _geometry = new video::Geometry();
  _material = new video::Material();

  _font = video::Font::get(_source, fmt::format("font::{}", _source));

  core::AutoPtr attrPosition = new video::Attribute(internal::rectPosition, 3);
  core::AutoPtr attrCoord = new video::Attribute(internal::rectTextureCoord, 2);
  core::AutoPtr attrIndices = new video::AttributeIndex(internal::rectIndices);

  _geometry->setAttribute(0, attrPosition);
  _geometry->setAttribute(1, attrCoord);
  _geometry->setAttributeIndex(attrIndices);

  _texture = new video::Texture(_font->getTexture());
  _material->setTexture("diffuse", _texture);

  _material->setIsTransparent(true);
  _material->setDepthTest(true);
  _material->setShader(_shader);

  _matrix = core::translate({0, 0, _zIndex});

  update();
  Node::onLoad();
}

void Font::onSourceChange() {
  _font = video::Font::get(_source, fmt::format("font::{}", _source));
  update();
}

void Font::onShaderChange() { _material->setShader(_shader); }

core::Unsigned_t Font::createText(const core::String_t &source) {
  static core::Unsigned_t handle = 0;
  static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  while (_texts.contains(handle)) {
    handle++;
  }
  _texts[handle] = {
      .text = converter.from_bytes(source),
      .position = {0, 0},
      .size = 24,
      .color = {0, 0, 0, 1.0f},
      .visible = true,
      .index = 0,
  };
  auto result = handle;
  handle++;
  update();
  return result;
}

void Font::setTextSource(core::Unsigned_t handle,
                         const core::String_t &source) {
  if (!_texts.contains(handle)) {
    return;
  }
  static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  _texts.at(handle).text = converter.from_bytes(source);
  update();
}
void Font::setTextSize(core::Unsigned_t handle, core::Unsigned_t size) {
  if (!_texts.contains(handle)) {
    return;
  }
  auto &text = _texts.at(handle);
  text.size = size;
  if (text.index < _matrixs.size()) {
    auto chrs = _font->draw(text.text);
    auto offset = 0;
    size_t index = 0;
    for (auto &chr : chrs) {
      _matrixs[text.index + index] =
          core::translate({(offset + text.position.x) * 1.0f,
                           text.position.y * 1.0f, _zIndex}) *
          core::scale({chr.advance * text.size / 64.0f, text.size, 1.0f});
      offset += chr.advance * text.size / 64.0f;
      index++;
    }
    _attrInstanceModel->write(text.index * sizeof(glm::mat4),
                              chrs.size() * sizeof(glm::mat4),
                              &_matrixs[text.index]);

  } else {
    update();
  }
}

void Font::setTextPosition(core::Unsigned_t handle,
                           const core::Point<> &position) {
  if (!_texts.contains(handle)) {
    return;
  }
  auto &text = _texts.at(handle);
  text.position = position;
  if (text.index < _matrixs.size()) {
    auto chrs = _font->draw(text.text);
    auto offset = 0;
    size_t index = 0;
    for (auto &chr : chrs) {
      _matrixs[text.index + index] =
          core::translate({(offset + text.position.x) * 1.0f,
                           text.position.y * 1.0f, _zIndex}) *
          core::scale({chr.advance * text.size / 64.0f, text.size, 1.0f});
      offset += chr.advance * text.size / 64.0f;
      index++;
    }
    _attrInstanceModel->write(text.index * sizeof(glm::mat4),
                              chrs.size() * sizeof(glm::mat4),
                              &_matrixs[text.index]);
  } else {
    update();
  }
}

void Font::setTextColor(core::Unsigned_t handle, const core::Color<> &color) {
  if (!_texts.contains(handle)) {
    return;
  }
  auto &text = _texts.at(handle);
  text.color = color;
  if (text.index < _matrixs.size()) {
    auto chrs = _font->draw(text.text);
    for (size_t index = 0; index < text.text.size(); index++) {
      _colors[text.index + index] = {text.color.r, text.color.g, text.color.b,
                                     text.color.a};
    }
    _attrInstanceColor->write(text.index * sizeof(glm::vec4),
                              chrs.size() * sizeof(glm::vec4),
                              &_colors[text.index]);
  } else {
    update();
  }
}

void Font::setTextVisible(core::Unsigned_t handle, core::Boolean_t visible) {
  if (!_texts.contains(handle)) {
    return;
  }
  _texts.at(handle).visible = visible;
  update();
}

void Font::setZIndex(core::Float_t zIndex) {
  _zIndex = zIndex;
  update();
}
void Font::deleteText(core::Unsigned_t handle) {
  _texts.erase(handle);
  update();
}

void Font::update() {
  _matrixs.clear();
  _coords.clear();
  _colors.clear();
  _matrix = core::translate({0, 0, _zIndex});
  core::Unsigned_t instanced = 0;
  for (auto &[_, text] : _texts) {
    if (!text.visible) {
      continue;
    }
    text.index = _matrixs.size();
    auto chrs = _font->draw(text.text);
    auto offset = 0;
    for (auto &chr : chrs) {
      _matrixs.pushBack(
          core::translate({(offset + text.position.x) * 1.0f,
                           text.position.y * 1.0f, _zIndex * 1.0f}) *
          core::scale({chr.advance * text.size / 64.0f, text.size, 1.0f}));

      _coords.pushBack(
          core::translate(
              {chr.offset * 1.0f / _font->getTexture()->getSize().width, 0,
               1.0f}) *
          core::scale(
              {chr.advance * 1.0f / _font->getTexture()->getSize().width, 1.0f,
               1.0f}));
      _colors.pushBack(
          {text.color.r, text.color.g, text.color.b, text.color.a});
      offset += chr.advance * text.size / 64.0f;
    }
    instanced += chrs.size();
  }
  if (!_material) {
    return;
  }
  _material->setInstanced(instanced);
  if (!_attrInstanceModel) {
    _attrInstanceModel = new video::Attribute(
        _matrixs.size() * sizeof(glm::mat4), (const void *)_matrixs.data(), 16);
    _geometry->setAttribute(2, _attrInstanceModel);
    _geometry->setVertexAttribDivisor(2);
    _geometry->setVertexAttribDivisor(3);
    _geometry->setVertexAttribDivisor(4);
    _geometry->setVertexAttribDivisor(5);
  } else {
    _attrInstanceModel->write(_matrixs.size() * sizeof(glm::mat4),
                              (const void *)_matrixs.data());
  }
  if (!_attrInstanceTexModel) {
    _attrInstanceTexModel = new video::Attribute(
        _coords.size() * sizeof(glm::mat4), (const void *)_coords.data(), 16);

    _geometry->setAttribute(6, _attrInstanceTexModel);
    _geometry->setVertexAttribDivisor(6);
    _geometry->setVertexAttribDivisor(7);
    _geometry->setVertexAttribDivisor(8);
    _geometry->setVertexAttribDivisor(9);
  } else {
    _attrInstanceTexModel->write(_coords.size() * sizeof(glm::mat4),
                                 (const void *)_coords.data());
  }

  if (!_attrInstanceColor) {
    _attrInstanceColor = new video::Attribute(
        _colors.size() * sizeof(glm::vec4), (const void *)_colors.data(), 4);

    _geometry->setAttribute(10, _attrInstanceColor);
    _geometry->setVertexAttribDivisor(10);
  } else {
    _attrInstanceColor->write(_colors.size() * sizeof(glm::vec4),
                              (const void *)_colors.data());
  }
}