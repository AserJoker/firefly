#include "video/Material.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
using namespace firefly;
using namespace firefly::video;
Material::Material()
    : _cullBackface(false), _alphaTest(false),
      _alphaFunc({gl::ALPHA_FUNC::ALWAYS, 0.0f}), _depthTest(true),
      _stencilTest(false), _blend(false),
      _blendFunc(
          {gl::BLEND_FUNC::SRC_ALPHA, gl::BLEND_FUNC::ONE_MINUS_SRC_ALPHA}),
      _visible(true), _instanced(1) {}

void Material::setShader(const std::string &shader) { _shader = shader; }
const std::string &Material::getShader() const { return _shader; }

const bool &Material::isWireframe() const { return _wireframe; }
const bool &Material::isCullBackface() const { return _cullBackface; }

const std::string &Material::getName() const { return _name; }

const uint32_t &Material::getInstanced() const { return _instanced; }

void Material::setIsWireframe(bool value) { _wireframe = value; }
void Material::setIsCullBackface(bool value) { _cullBackface = value; }
void Material::setName(const std::string &name) { _name = name; }
const bool &Material::isBlend() const { return _blend; }
void Material::setBlend(bool value) { _blend = value; }

const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &
Material::getBlendFunc() const {
  return _blendFunc;
}
void Material::setBlendFunc(
    const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &func) {
  _blendFunc = func;
}

const bool &Material::isVisible() const { return _visible; }
void Material::setVisible(bool value) { _visible = value; }

const bool &Material::isStencilTest() const { return _stencilTest; }
void Material::setStencil(bool value) { _stencilTest = value; }

const bool &Material::isDepthTest() const { return _depthTest; }
void Material::setDepthTest(bool value) { _depthTest = value; }

const bool &Material::isAlphaTest() const { return _alphaTest; }
void Material::setAlphaTest(bool value) { _alphaTest = value; }

const std::pair<gl::ALPHA_FUNC, float> &Material::getAlphaFunc() const {
  return _alphaFunc;
}
void Material::setAlphaFunc(const std::pair<gl::ALPHA_FUNC, float> &func) {
  _alphaFunc = func;
}

void Material::enableAttribute(const std::string &name) {
  _enableAttributes.insert(name);
}
void Material::disableAttribute(const std::string &name) {
  _enableAttributes.erase(name);
}
