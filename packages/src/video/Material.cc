#include "video/Material.hpp"
#include "core/AutoPtr.hpp"
#include <unordered_map>
using namespace firefly;
using namespace firefly::video;
void Material::setTexture(const uint32_t &id, const std::string &name) {
  setVersion(getVersion() + 1);
}
const core::AutoPtr<Image> Material::getTexture(const uint32_t &id) const {
  if (_textures.contains(id)) {
    return _textures.at(id);
  }
  return nullptr;
}