#include "video/Material.hpp"
using namespace firefly;
using namespace firefly::video;
Material::Material(const std::string &type)
    : _type(type), _alphaTest(false), _depthTest(true), _stencilTest(false),
      _blend(false), _visible(true) {}
const std::string &Material::getType() const { return _type; }
const std::unordered_map<std::string, std::string> &
Material::getTextures() const {
  return _textures;
}
void Material::setTexture(const std::string &name, const std::string &texture) {
  _textures[name] = texture;
}
void Material::active(core::AutoPtr<Constant> &constants) const {
  auto index = 0;
  for (auto &[name, _] : _textures) {
    constants->setField(name, index++);
  }
}

const bool &Material::isBlend() const { return _blend; }
void Material::setBlend(const bool &value) { _blend = value; }

const bool &Material::isVisible() const { return _visible; }
void Material::setVisible(const bool &value) { _visible = value; }

const bool &Material::isStencilTest() const { return _stencilTest; }
void Material::setStencil(const bool &value) { _stencilTest = value; }

const bool &Material::isDepthTest() const { return _depthTest; }
void Material::setDepthTest(const bool &value) { _depthTest = value; }

const bool &Material::isAlphaTest() const { return _alphaTest; }
void Material::setAlphaTest(const bool &value) { _alphaTest = value; }