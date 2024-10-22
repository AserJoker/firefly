#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "core/AutoPtr.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Rect.hpp"
#include "core/Value.hpp"
#include "gl/Program.hpp"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <list>

namespace firefly::video {
class Renderer : public core::Object {
private:
  struct Pass {
    core::AutoPtr<Geometry> geometry;
    core::AutoPtr<Material> material;
    glm::mat4 matrixModel;
  };
  struct RenderContext : public core::Object {
    std::list<Pass> solidRenderList;
    std::list<Pass> blendRenderList;
  };

private:
  std::string _shaderName;

  core::AutoPtr<gl::Program> _shader;

  core::AutoPtr<RenderContext> _context;

  core::Rect<> _viewport;

  core::Map<core::String_t, core::AutoPtr<Texture>> _textures;

  core::Map<std::string, gl::Uniform> _uniforms;

  SDL_GLContext _ctx;

  SDL_Window *_window;

private:
  void drawPass(const Pass &item);

  void drawPassList(std::list<Pass> &list);

  void drawContext(core::AutoPtr<RenderContext> &ctx);

  void setMaterial(const core::AutoPtr<Material> &material);

  bool activeShader(const std::string &name, const std::string &stage);

  void bindingTextures(
      const core::Map<core::String_t, core::AutoPtr<Texture>> &textures,
      uint32_t offset = 0);

public:
  Renderer(SDL_Window *window);
  ~Renderer() override;

  void draw(const core::AutoPtr<Material> &material,
            const core::AutoPtr<Geometry> &geometry, const glm::mat4 &matrix);

  void setShader(const std::string &name);

  const std::string &getShader() const;

  void setViewport(const core::Rect<> &viewport);

  const core::Rect<> &getViewport() const;

  core::AutoPtr<gl::Program> getShaderProgram();

  core::AutoPtr<RenderContext> pushContext();

  void popContext(const core::AutoPtr<RenderContext> &ctx);

  void setUniform(const std::string &name, const gl::Uniform &uniform);

  void setTexture(const core::String_t &name, const core::String_t &path);

  void removeTexture(const core::String_t &name);

  const core::Map<core::String_t, core::AutoPtr<Texture>> &getTextures() const;

  void present();
};
} // namespace firefly::video