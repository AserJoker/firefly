#include "video/Material.hpp"
#include "core/AutoPtr.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Constant.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/Logger.hpp"
using namespace firefly;
using namespace firefly::video;
Material::Material()
    : _ambient(0.0f), _diffuse(0.0f), _specular(0.0f), _wireframe(false),
      _cullBackface(false), _alphaTest(false),
      _alphaFunc({gl::ALPHA_FUNC::ALWAYS, 0.0f}), _depthTest(true),
      _stencilTest(false), _blend(false),
      _blendFunc(
          {gl::BLEND_FUNC::SRC_ALPHA, gl::BLEND_FUNC::ONE_MINUS_SRC_ALPHA}),
      _visible(true), _instanced(1) {

  enableAttribute(DIFFUSE_TEX);
}

void Material::setShader(const std::string &shader) { _shader = shader; }
const std::string &Material::getShader() const { return _shader; }

const std::unordered_map<std::string, Material::TextureInfo> &
Material::getTextures() const {
  return _textures;
}
std::unordered_map<std::string, Material::TextureInfo> &
Material::getTextures() {
  return _textures;
}
void Material::setTexture(const std::string &name, const std::string &path,
                          const glm::mat4 &textureCoordMatrix, float blend,
                          gl::TEXTURE_WRAP_MODE mappingmodeU,
                          gl::TEXTURE_WRAP_MODE mappingmodeV) {
  auto p = fmt::format("texture::{}", path);
  core::AutoPtr<gl::Texture2D> tex;
  try {
    tex = gl::Texture2D::get(path, p, mappingmodeU, mappingmodeV);
  } catch (std::exception &e) {
    auto logger = core::Singleton<runtime::Logger>::instance();
    logger->warn("Failed to load texture '{}.{}'\nCaused by:\n\t{}", _name,
                 name, e.what());
  }
  if (!tex) {
    tex = gl::Texture2D::get("internal");
    gl::Texture2D::set(path, tex);
  }
  setTexture(name, tex, textureCoordMatrix, blend);
}
void Material::setTexture(const std::string &name,
                          const core::AutoPtr<gl::Texture2D> &tex,
                          const glm::mat4 &textureCoordMatrix, float blend) {
  _textures[name] = {tex, blend, textureCoordMatrix};
}

const glm::vec3 &Material::getAmbient() const { return _ambient; }
const glm::vec3 &Material::getDiffuse() const { return _diffuse; }
const glm::vec3 &Material::getSpecular() const { return _specular; }
const bool &Material::isWireframe() const { return _wireframe; }
const bool &Material::isCullBackface() const { return _cullBackface; }

const std::string &Material::getName() const { return _name; }

void Material::setAmbient(const glm::vec3 &color) {
  _ambient = color;
  if (!_attributes.contains(AMBIENT_COLOR)) {
    _attributes[AMBIENT_COLOR] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("ambient", _ambient);
    };
  }
}

const uint32_t &Material::getInstanced() const { return _instanced; }

void Material::setDiffuse(const glm::vec3 &color) {
  _diffuse = color;
  if (!_attributes.contains(AMBIENT_COLOR)) {
    _attributes[AMBIENT_COLOR] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("diffuse", _diffuse);
    };
  }
}

void Material::setSpecular(const glm::vec3 &color) {
  _specular = color;
  if (!_attributes.contains(SPECULAR_COLOR)) {
    _attributes[SPECULAR_COLOR] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("specular", _specular);
    };
  }
}
void Material::setIsWireframe(bool value) { _wireframe = value; }
void Material::setIsCullBackface(bool value) { _cullBackface = value; }
void Material::setName(const std::string &name) { _name = name; }

void Material::setInstanced(uint32_t instanced) { _instanced = instanced; }

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
const std::unordered_map<std::string, Material::Attribute> &
Material::getAttributes() const {
  return _attributes;
}
