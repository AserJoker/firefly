#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Shader.hpp"
#include "video/Camera.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Object3D.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace firefly::video {
class Renderer : public core::Object {
private:
  std::unordered_map<std::string, core::AutoPtr<core::Object>> _cache;
  core::AutoPtr<gl::Shader> _shader;

private:
  void syncToBackend(core::AutoPtr<Geometry> &geometry);
  void syncToBackend(core::AutoPtr<Image> &image);

public:
  Renderer();
  void renderGeometry(core::AutoPtr<Geometry> &geometry);
  void renderGeometryInstance(core::AutoPtr<Geometry> &geometry,
                              const uint32_t &count);
  void setMaterial(core::AutoPtr<Material> &mat);

  void render(core::AutoPtr<Camera> camera, core::AutoPtr<Object3D> root);

  core::AutoPtr<gl::Shader> getShader();
  void setShader(const core::AutoPtr<gl::Shader> &shader);
};
} // namespace firefly::video