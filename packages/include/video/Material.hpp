#pragma once
#include "Texture.hpp"
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Value.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Uniform.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

namespace firefly::video {
class Material : public core::Object, public core::Cache<Material> {
public:
  using Attribute = gl::Uniform;

private:
  core::Map<core::String_t, Attribute> _attributes;

  core::Map<core::String_t, core::AutoPtr<Texture>> _textures;

  core::String_t _name;

  core::String_t _shader;

  core::Boolean_t _wireframe;

  core::Boolean_t _transparent;

  core::Boolean_t _cullBackface;

  core::Boolean_t _alphaTest;
  std::pair<gl::ALPHA_FUNC, core::Float_t> _alphaFunc;

  core::Boolean_t _depthTest;

  core::Boolean_t _stencilTest;

  std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> _blendFunc;

  core::Boolean_t _visible;

  core::Unsigned_t _instanced;

public:
  Material();
  void setShader(const core::String_t &shader);
  const core::String_t &getShader() const;

  void setIsWireframe(core::Boolean_t value);
  core::Boolean_t isWireframe() const;

  void setIsTransparent(core::Boolean_t value);
  core::Boolean_t isTransparent() const;

  void setIsCullBackface(core::Boolean_t value);
  core::Boolean_t isCullBackface() const;

  void setName(const core::String_t &name);
  const core::String_t &getName() const;

  void setInstanced(core::Unsigned_t instanced);
  const core::Unsigned_t &getInstanced() const;

  const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &getBlendFunc() const;
  void setBlendFunc(const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &func);

  core::Boolean_t isVisible() const;
  void setVisible(core::Boolean_t value);

  core::Boolean_t isStencilTest() const;
  void setStencil(core::Boolean_t value);

  core::Boolean_t isDepthTest() const;
  void setDepthTest(core::Boolean_t value);

  core::Boolean_t isAlphaTest() const;
  void setAlphaTest(core::Boolean_t value);

  const std::pair<gl::ALPHA_FUNC, core::Float_t> &getAlphaFunc() const;
  void setAlphaFunc(const std::pair<gl::ALPHA_FUNC, core::Float_t> &func);

  void setAttribute(const core::String_t &name, const Attribute &value);
  const Attribute &getAttribute(const core::String_t &name) const;
  const core::Map<core::String_t, Attribute> &getAttributes() const;

  void setTexture(const core::String_t &name,
                  const core::AutoPtr<Texture> &path);
  const core::AutoPtr<Texture> &getTexture(const core::String_t &name) const;
  const core::Map<core::String_t, core::AutoPtr<Texture>> &getTextures() const;
};
} // namespace firefly::video