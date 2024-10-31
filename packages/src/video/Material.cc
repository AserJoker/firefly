#include "video/Material.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
using namespace firefly;
using namespace firefly::video;
Material::Material()
    : _wireframe(false), _transparent(false), _cullBackface(false),
      _alphaTest(false), _alphaFunc({gl::ALPHA_FUNC::ALWAYS, 0.0f}),
      _depthTest(true), _stencilTest(false),
      _blendFunc(
          {gl::BLEND_FUNC::SRC_ALPHA, gl::BLEND_FUNC::ONE_MINUS_SRC_ALPHA}),
      _visible(true), _instanced(1) {}

void Material::setShader(const core::String_t &shader) { _shader = shader; }

const core::String_t &Material::getShader() const { return _shader; }

void Material::setIsWireframe(core::Boolean_t value) { _wireframe = value; }

core::Boolean_t Material::isWireframe() const { return _wireframe; }

void Material::setIsCullBackface(core::Boolean_t value) {
  _cullBackface = value;
}

core::Boolean_t Material::isCullBackface() const { return _cullBackface; }

void Material::setIsTransparent(core::Boolean_t value) { _transparent = value; }

core::Boolean_t Material::isTransparent() const { return _transparent; }

const core::String_t &Material::getName() const { return _name; }

void Material::setInstanced(core::Unsigned_t instanced) {
  _instanced = instanced;
}

const core::Unsigned_t &Material::getInstanced() const { return _instanced; }

void Material::setName(const core::String_t &name) { _name = name; }

const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &
Material::getBlendFunc() const {
  return _blendFunc;
}

void Material::setBlendFunc(
    const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &func) {
  _blendFunc = func;
}

core::Boolean_t Material::isVisible() const { return _visible; }

void Material::setVisible(core::Boolean_t value) { _visible = value; }

core::Boolean_t Material::isStencilTest() const { return _stencilTest; }

void Material::setStencil(core::Boolean_t value) { _stencilTest = value; }

core::Boolean_t Material::isDepthTest() const { return _depthTest; }

void Material::setDepthTest(core::Boolean_t value) { _depthTest = value; }

core::Boolean_t Material::isAlphaTest() const { return _alphaTest; }

void Material::setAlphaTest(core::Boolean_t value) { _alphaTest = value; }

const std::pair<gl::ALPHA_FUNC, core::Float_t> &Material::getAlphaFunc() const {
  return _alphaFunc;
}

void Material::setAlphaFunc(
    const std::pair<gl::ALPHA_FUNC, core::Float_t> &func) {
  _alphaFunc = func;
}

void Material::setAttribute(const core::String_t &name,
                            const Material::Attribute &value) {
  _attributes[name] = value;
}

const Material::Attribute &
Material::getAttribute(const core::String_t &name) const {
  return _attributes[name];
}

const core::Map<core::String_t, Material::Attribute> &
Material::getAttributes() const {
  return _attributes;
}

void Material::setTexture(const core::String_t &name,
                          const core::AutoPtr<Texture> &texture) {
  _textures[name] = texture;
}

const core::AutoPtr<Texture> &
Material::getTexture(const core::String_t &name) const {
  return _textures[name];
}

const core::Map<core::String_t, core::AutoPtr<Texture>> &
Material::getTextures() const {
  return _textures;
}