#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Shader.hpp"
#include "video/Camera.hpp"
#include "video/Geometry.hpp"
#include "video/Meterial.hpp"
#include "video/Object3D.hpp"
#include <glm/glm.hpp>

namespace firefly::video {
class Renderer : public core::Object {
private:
  core::AutoPtr<gl::Shader> _shader;

private:
  void syncToBackend(core::AutoPtr<Geometry> &geometry);

public:
  Renderer();
  void renderGeometry(core::AutoPtr<Geometry> &geometry);
  void renderGeometryInstance(core::AutoPtr<Geometry> &geometry,
                              const uint32_t &count);

  void setMatrial(const core::AutoPtr<Material> &mat);
  void render(core::AutoPtr<Camera> camera, core::AutoPtr<Object3D> root);

  core::AutoPtr<gl::Shader> getShader();
  void setShader(const core::AutoPtr<gl::Shader> &shader);
};
} // namespace firefly::video