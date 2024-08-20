#pragma once
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::video {
class Device : public core::Object {
public:
  void clear();
  void setClearColor(float r, float g, float b, float a);
  void enable(uint32_t state);
  void disable(uint32_t state);
  void setShader(const core::AutoPtr<Shader> &shader);
  void setTexture(const core::AutoPtr<Texture> &texture, uint32_t index = 0);
  void draw(uint32_t mode, const core::AutoPtr<Mesh> &mesh);
};
}; // namespace firefly::video