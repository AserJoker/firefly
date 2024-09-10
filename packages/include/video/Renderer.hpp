#pragma once
#include "Camera.hpp"
#include "Constant.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "ModelSet.hpp"
#include "RenderObject.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Program.hpp"
#include <vector>
namespace firefly::video {
class Renderer : public core::Object {
private:
  core::AutoPtr<gl::Program> _shader;
  core::AutoPtr<Constant> _constants;
  std::vector<core::AutoPtr<RenderObject>> _normalRenderList;
  std::vector<core::AutoPtr<RenderObject>> _blendRenderList;

private:
  void syncConstats(bool force);

public:
  Renderer();
  void drawGeomeory(const core::AutoPtr<Geometry> &geometry);
  void setShader(const std::string &name);
  void setMaterial(const core::AutoPtr<Material> &material);
  const core::AutoPtr<Constant> &getConstants() const;
  core::AutoPtr<Constant> &getConstants();
  void draw(const core::AutoPtr<Material> &material,
            const core::AutoPtr<Geometry> &geometry);
  void draw(const core::AutoPtr<Model> &mesh);
  void draw(const core::AutoPtr<ModelSet> &modelset);
  void begin(const core::AutoPtr<Camera> &camera);
  void end();
};
} // namespace firefly::video