#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Rect.hpp"
#include "gl/Program.hpp"
#include "gl/Texture2D.hpp"
#include "video/RenderTarget.hpp"
#include <glm/glm.hpp>
#include <list>
#include <unordered_map>

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

  core::AutoPtr<RenderContext> _context;

  core::Rect<> _viewport;

  core::AutoPtr<RenderTarget> _deferred;
  std::list<core::AutoPtr<RenderTarget>> _shaderRenderTargets;
  core::Map<std::string, core::AutoPtr<gl::Texture2D>> _textures;

  std::unordered_map<std::string, gl::Uniform> _uniforms;

private:
  void draw(const RenderItem &item);

public:
  Renderer();
  void draw(const core::AutoPtr<Material> &material,
            const core::AutoPtr<Geometry> &geometry, const glm::mat4 &matrix);
  using core::Object::initialize;
  void initialize(const core::Rect<> &viewport);

  bool activeShader(const std::string &name, const std::string &stage);
  void setShader(const std::string &name);
  const std::string &getShader() const;

  void setMaterial(const core::AutoPtr<Material> &material);

  void setViewport(const core::Rect<> &viewport);
  const core::Rect<> &getViewport() const;

  core::AutoPtr<gl::Program> getShaderProgram();

  core::AutoPtr<RenderContext> pushContext();
  void popContext(const core::AutoPtr<RenderContext> &ctx);
  void setTexture(const std::string &name,
                  core::AutoPtr<gl::Texture2D> texture);
  void clearTexture(const std::string &name);
  void present();
  void setUniform(const std::string &name, const gl::Uniform &uniform);
};
} // namespace firefly::video