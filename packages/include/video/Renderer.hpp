#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Shader.hpp"
#include "video/Texture.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Renderer : public core::Object {
public:
  void clear(const glm::vec4 &color);
  void setTextureUnit(core::AutoPtr<Shader> shader, const std::string &name,
                      const int32_t id, core::AutoPtr<Texture> texture);
};
} // namespace firefly::video