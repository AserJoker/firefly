#include "video/Material.hpp"
using namespace firefly;
using namespace firefly::video;
Material::Material(const std::string &type) : _type(type) {}
const std::string &Material::getType() const { return _type; }
const std::unordered_map<std::string, core::AutoPtr<Image>> &
Material::getTextures() const {
  return _textures;
}
void Material::setTexture(const std::string &name,
                          const core::AutoPtr<Image> &texture) {
  _textures[name] = texture;
}
void Material::active(core::AutoPtr<Constant> &constants) const {
  auto index = 0;
  for (auto &[name, _] : _textures) {
    constants->setField(name, index++);
  }
}
void Material::setShader(const core::AutoPtr<Shader> &shader) {
  _shader = shader;
}
const core::AutoPtr<Shader> &Material::getShader() const { return _shader; }