#pragma once
#include "Device.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace firefly::video {
class Renderer2D : public core::Object {
private:
  glm::mat4 _projection;

  core::AutoPtr<Device> _device;
  core::AutoPtr<Mesh> _mesh2D;
  core::AutoPtr<Shader> _shader2D;

public:
  void draw(const core::AutoPtr<Texture> &texture, const glm::vec4 &source,
            const glm::vec4 &target, const glm::vec2 center, float angle);
  
  void setViewport(float left, float right, float top, float bottom,
                   float near = -1, float far = 1);
};
}; // namespace firefly::video