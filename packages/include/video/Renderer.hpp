#pragma once
#include "Camera.hpp"
#include "Geometry.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "ModelSet.hpp"
#include "RenderObject.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Constant.hpp"
#include "gl/Program.hpp"
#include "video/RenderTarget.hpp"
#include <glm/glm.hpp>
#include <list>

namespace firefly::video {
class Renderer : public core::Object {
public:
private:
  std::string _shaderName;
  core::AutoPtr<gl::Program> _shader;

  core::AutoPtr<gl::Constant> _constants;

  std::list<core::AutoPtr<RenderObject>> _normalRenderList;
  std::list<core::AutoPtr<RenderObject>> _blendRenderList;

  core::AutoPtr<Light> _light;

  glm::ivec4 _viewport;

  core::AutoPtr<RenderTarget> _deferred;
  std::list<core::AutoPtr<RenderTarget>> _shaderRenderTargets;
  core::AutoPtr<RenderTarget> _renderTarget;

public:
  Renderer();
  using core::Object::initialize;
  void initialize(const glm::ivec4 &viewport);

  bool activeShader(const std::string &name, const std::string &stage);
  void setShader(const std::string &name);
  const std::string &getShader() const;

  void setMaterial(const core::AutoPtr<Material> &material);

  const core::AutoPtr<gl::Constant> &getConstants() const;
  core::AutoPtr<gl::Constant> &getConstants();

  core::AutoPtr<Light> &getLight();
  const core::AutoPtr<Light> &getLight() const;

  void setViewport(const glm::ivec4 &viewport);
  const glm::ivec4 &getViewport() const;

  core::AutoPtr<gl::Program> getShaderProgram();

  void draw(const core::AutoPtr<Material> &material,
            const core::AutoPtr<Geometry> &geometry, const glm::mat4 &matrix);

  void draw(const core::AutoPtr<ModelSet> &modelset, const glm::mat4 &model);

  void begin(const core::AutoPtr<Camera> &camera);
  void end();

  void setRenderTarget(const core::AutoPtr<RenderTarget> &target);
};
} // namespace firefly::video