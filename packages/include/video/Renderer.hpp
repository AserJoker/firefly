#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Camera.hpp"
#include "video/Mesh.hpp"
#include "video/Shader.hpp"
#include "video/Texture.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Renderer : public core::Object {
private:
  core::AutoPtr<Shader> _shader;
  core::AutoPtr<Camera> _camera;

public:
  void setCamera(const core::AutoPtr<Camera> &camera);
  void useShader(const core::AutoPtr<Shader> &shader);
  const core::AutoPtr<Shader> &getShader();
  void clear(const glm::vec4 &color);
  void setTextureUnit(const std::string &name, const int32_t id,
                      const core::AutoPtr<Texture> &texture);
  void enableDepthTest();
  void disableDepthTest();
  void draw(const core::AutoPtr<Mesh> &mesh);
};
} // namespace firefly::video