#pragma once
#include "Camera.hpp"
#include "Geometry.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "ModelSet.hpp"
#include "RenderObject.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Constant.hpp"
#include "gl/Program.hpp"
#include "runtime/Event_Resize.hpp"
#include <glm/fwd.hpp>
#include <vector>

namespace firefly::video {
class Renderer : public core::Object {
public:
private:
  std::string _shaderName;
  core::AutoPtr<gl::Program> _shader;
  core::AutoPtr<gl::Constant> _constants;
  std::vector<core::AutoPtr<RenderObject>> _normalRenderList;
  std::vector<core::AutoPtr<RenderObject>> _blendRenderList;
  core::AutoPtr<Light> _light;
  glm::ivec4 _viewport;

private:
  bool activeShader(const std::string &name, const std::string &stage);
  void onWindowResize(const runtime::Event_Resize &event);

public:
  Renderer();
  void setShader(const std::string &name);
  void setMaterial(const core::AutoPtr<Material> &material);
  const core::AutoPtr<gl::Constant> &getConstants() const;
  core::AutoPtr<gl::Constant> &getConstants();
  core::AutoPtr<Light> &getLight();
  void draw(const core::AutoPtr<Material> &material,
            const core::AutoPtr<Geometry> &geometry);
  void draw(const core::AutoPtr<Model> &mesh);
  void draw(const core::AutoPtr<ModelSet> &modelset);
  void begin(const core::AutoPtr<Camera> &camera);
  void end();
};
} // namespace firefly::video