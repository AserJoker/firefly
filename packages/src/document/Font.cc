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
Font::Font() {
  _shader = "font";
  _source = "simhei.ttf";

  defineProperty(PROP_SOURCE, _source);
  defineProperty(PROP_SHADER, _shader);

  watchProp(PROP_SOURCE, &Font::onSourceChange);
  watchProp(PROP_SHADER, &Font::onShaderChange);
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

  update();
}

void Font::onSourceChange() {
  _font = video::Font::get(_source, fmt::format("font::{}", _source));
  update();
}

void Font::onShaderChange() { _material->setShader(_shader); }

uint32_t Font::createText(const core::String_t &source) {
  static uint32_t handle = 0;
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
  };
  auto result = handle;
  handle++;
  update();
  return result;
}

void Font::setTextSource(uint32_t handle, const core::String_t &source) {
  if (!_texts.contains(handle)) {
    return;
  }
  static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  _texts.at(handle).text = converter.from_bytes(source);
  update();
}
void Font::setTextSize(uint32_t handle, uint32_t size) {
  if (!_texts.contains(handle)) {
    return;
  }
  auto &text = _texts.at(handle);
  text.size = size;
  update();
}

void Font::setTextPosition(uint32_t handle, const core::Point<> &position) {
  if (!_texts.contains(handle)) {
    return;
  }
  _texts.at(handle).position = position;
  update();
}

void Font::setTextColor(uint32_t handle, const core::Color<> &color) {
  if (!_texts.contains(handle)) {
    return;
  }
  _texts.at(handle).color = color;
  update();
}

void Font::setTextVisible(uint32_t handle, bool visible) {
  if (!_texts.contains(handle)) {
    return;
  }
  _texts.at(handle).visible = visible;
  update();
}

void Font::deleteText(uint32_t handle) {
  _texts.erase(handle);
  update();
}

void Font::update() {
  _matrixs.clear();
  _coords.clear();
  _colors.clear();
  uint32_t instanced = 0;
  for (auto &[_, text] : _texts) {
    if (!text.visible) {
      continue;
    }
    auto chrs = _font->draw(text.text);
    auto offset = 0;
    for (auto &chr : chrs) {
      _matrixs.pushBack(
          core::translate({(offset + text.position.x) * 1.0f,
                           text.position.y * 1.0f, 1.0f}) *
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