#include "video/Material.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Constant.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/Logger.hpp"
using namespace firefly;
using namespace firefly::video;
Material::Material()
    : _ambient(0.0f), _diffuse(0.0f), _specular(0.0f), _emissive(0.0f),
      _reflective(0.0f), _transparent(0.0f), _reflectivity(0.0f),
      _wireframe(false), _cullBackface(false), _blendAdd(false), _opacity(1.0f),
      _shininess(0.0f), _shininessStrength(1.0f), _alphaTest(false),
      _depthTest(true), _stencilTest(false), _blend(false), _visible(true) {

  enableAttribute(DIFFUSE_TEX);
}

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
const glm::vec3 &Material::getEmissive() const { return _emissive; }
const glm::vec3 &Material::getReflective() const { return _reflective; }
const glm::vec3 &Material::getTransparent() const { return _transparent; }
const float &Material::getReflectivity() const { return _reflectivity; }
const bool &Material::isWireframe() const { return _wireframe; }
const bool &Material::isCullBackface() const { return _cullBackface; }
const bool &Material::isBlendAdd() const { return _blendAdd; }
const float &Material::getOpacity() const { return _opacity; }
const float &Material::getShininess() const { return _shininess; }
const float &Material::getShininessStrength() const {
  return _shininessStrength;
}

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
void Material::setEmissive(const glm::vec3 &color) {
  _emissive = color;
  if (!_attributes.contains(EMISSIVE_COLOR)) {
    _attributes[EMISSIVE_COLOR] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("emissive", _emissive);
    };
  }
}
void Material::setReflective(const glm::vec3 &color) {
  _reflective = color;
  if (!_attributes.contains(REFLECTIVE_COLOR)) {
    _attributes[REFLECTIVE_COLOR] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("reflective", _reflective);
    };
  }
}
void Material::setTransparent(const glm::vec3 &color) {
  _transparent = color;
  if (!_attributes.contains(TRANSPARENT_COLOR)) {
    _attributes[TRANSPARENT_COLOR] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("transparent", _transparent);
    };
  }
}
void Material::setReflectivity(float value) {
  _reflectivity = value;
  if (!_attributes.contains(REFLECTIVITY)) {
    _attributes[REFLECTIVITY] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("reflectivity", _reflectivity);
    };
  }
}
void Material::setIsWireframe(bool value) { _wireframe = value; }
void Material::setIsCullBackface(bool value) { _cullBackface = value; }
void Material::setIsBlendAdd(bool value) { _blendAdd = value; }
void Material::setOpacity(float value) {
  _opacity = value;
  if (_attributes.contains(OPACITY)) {
    _attributes[OPACITY] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("opacity", _opacity);
    };
  }
}
void Material::setShininess(float value) {
  _shininess = value;
  if (_attributes.contains(SHININESS)) {
    _attributes[SHININESS] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("shininess", _shininess);
    };
  }
}
void Material::setShininessStrength(float value) {
  _shininessStrength = value;
  if (_attributes.contains(SHININESS_STRENGTH)) {
    _attributes[SHININESS_STRENGTH] =
        [this](core::AutoPtr<gl::Constant> constants) -> void {
      constants->setField("shininess_strength", _shininessStrength);
    };
  }
}
void Material::setName(const std::string &name) { _name = name; }

const bool &Material::isBlend() const { return _blend; }
void Material::setBlend(bool value) { _blend = value; }

const bool &Material::isVisible() const { return _visible; }
void Material::setVisible(bool value) { _visible = value; }

const bool &Material::isStencilTest() const { return _stencilTest; }
void Material::setStencil(bool value) { _stencilTest = value; }

const bool &Material::isDepthTest() const { return _depthTest; }
void Material::setDepthTest(bool value) { _depthTest = value; }

const bool &Material::isAlphaTest() const { return _alphaTest; }
void Material::setAlphaTest(bool value) { _alphaTest = value; }
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