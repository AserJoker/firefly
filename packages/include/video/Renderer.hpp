#pragma once
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Geometry.hpp"
#include <unordered_map>
namespace firefly::video {
class Object3D;
class Renderer : public core::Object {

private:
  core::AutoPtr<Shader> _shader;

private:
  void syncToBackend(core::AutoPtr<Geometry> &geometry);

public:
  void setShader(const core::AutoPtr<Shader> &shader);
  void renderGeometry(core::AutoPtr<Geometry> &geometry);
  void render(core::AutoPtr<Object3D> root);
};
} // namespace firefly::video