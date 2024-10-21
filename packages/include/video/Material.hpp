#pragma once
#include "core/Cache.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Value.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Uniform.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>

namespace firefly::video {
class Material : public core::Object, public core::Cache<Material> {
public:
  using Attribute = gl::Uniform;

private:
  core::Map<core::String_t, Attribute> _attributes;

  core::Map<core::String_t, core::String_t> _textures;

  std::string _name;

  std::string _shader;

  bool _wireframe;

  bool _transparent;

  bool _cullBackface;

  bool _alphaTest;
  std::pair<gl::ALPHA_FUNC, float> _alphaFunc;

  bool _depthTest;

  bool _stencilTest;

  std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> _blendFunc;

  bool _visible;

  uint32_t _instanced;

public:
  Material();
  void setShader(const std::string &shader);
  const std::string &getShader() const;

  void setIsWireframe(bool value);
  bool isWireframe() const;

  void setIsTransparent(bool value);
  bool isTransparent() const;

  void setIsCullBackface(bool value);
  bool isCullBackface() const;

  void setName(const std::string &name);
  const std::string &getName() const;

  void setInstanced(uint32_t instanced);
  const uint32_t &getInstanced() const;

  const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &getBlendFunc() const;
  void setBlendFunc(const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &func);

  const bool &isVisible() const;
  void setVisible(bool value);

  const bool &isStencilTest() const;
  void setStencil(bool value);

  const bool &isDepthTest() const;
  void setDepthTest(bool value);

  const bool &isAlphaTest() const;
  void setAlphaTest(bool value);

  const std::pair<gl::ALPHA_FUNC, float> &getAlphaFunc() const;
  void setAlphaFunc(const std::pair<gl::ALPHA_FUNC, float> &func);

  void setAttribute(const core::String_t &name, const Attribute &value);
  const Attribute &getAttribute(const core::String_t &name) const;
  const core::Map<core::String_t, Attribute> &getAttributes() const;

  void setTexture(const core::String_t &name, const core::String_t &path);
  const core::String_t &getTexture(const core::String_t &name) const;
  const core::Map<core::String_t, core::String_t> &getTextures() const;
};
} // namespace firefly::video