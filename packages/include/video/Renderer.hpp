#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Constant.hpp"
#include "gl/Program.hpp"
#include "video/RenderTarget.hpp"
#include <glm/glm.hpp>
#include <list>

namespace firefly::video {
class Renderer : public core::Object {
private:
  struct RenderItem {
    core::AutoPtr<Geometry> geometry;
    core::AutoPtr<Material> material;
    glm::mat4 matrixModel;
  };
  struct RenderContext : public core::Object {
    std::list<RenderItem> normalRenderList;
    std::list<RenderItem> blendRenderList;
  };

private:
  std::string _shaderName;
  core::AutoPtr<gl::Program> _shader;

  core::AutoPtr<gl::Constant> _constants;

  core::AutoPtr<RenderContext> _context;

  glm::ivec4 _viewport;

  core::AutoPtr<RenderTarget> _deferred;
  std::list<core::AutoPtr<RenderTarget>> _shaderRenderTargets;

private:
public:
  Renderer();
  void draw(const core::AutoPtr<Material> &material,
            const core::AutoPtr<Geometry> &geometry, const glm::mat4 &matrix);
  using core::Object::initialize;
  void initialize(const glm::ivec4 &viewport);

  bool activeShader(const std::string &name, const std::string &stage);
  void setShader(const std::string &name);
  const std::string &getShader() const;

  void setMaterial(const core::AutoPtr<Material> &material);

  const core::AutoPtr<gl::Constant> &getConstants() const;
  core::AutoPtr<gl::Constant> &getConstants();

  void setViewport(const glm::ivec4 &viewport);
  const glm::ivec4 &getViewport() const;

  core::AutoPtr<gl::Program> getShaderProgram();

  void present();
  core::AutoPtr<RenderContext> pushContext();
  void popContext(const core::AutoPtr<RenderContext> &ctx);
};
} // namespace firefly::video