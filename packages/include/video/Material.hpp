#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Image.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
namespace firefly::video {
class Material : public core::Object {
private:
  std::unordered_map<uint32_t, core::AutoPtr<Image>> _textures;

public:
  void setTexture(const uint32_t &id, const std::string &name);
  const core::AutoPtr<Image> getTexture(const uint32_t &id) const;
};
}; // namespace firefly::video