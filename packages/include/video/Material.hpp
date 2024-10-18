#pragma once
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include <glm/glm.hpp>
#include <set>

namespace firefly::video {
class Material : public core::Object, public core::Cache<Material> {
public:
private:
  std::set<std::string> _enableAttributes;

  std::string _name;
  std::string _shader;

  bool _wireframe;
  bool _cullBackface;

  bool _alphaTest;
  std::pair<gl::ALPHA_FUNC, float> _alphaFunc;
  bool _depthTest;
  bool _stencilTest;

  bool _blend;
  std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> _blendFunc;
  bool _visible;

  uint32_t _instanced;

public:
  Material();
  void setShader(const std::string &shader);
  const std::string &getShader() const;

  void setIsWireframe(bool value);
  const bool &isWireframe() const;

  void setIsCullBackface(bool value);
  const bool &isCullBackface() const;

  void setName(const std::string &name);
  const std::string &getName() const;

  void setInstanced(uint32_t instanced);
  const uint32_t &getInstanced() const;

  const bool &isBlend() const;
  void setBlend(bool value);

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

  void enableAttribute(const std::string &name);
  void disableAttribute(const std::string &name);
};
} // namespace firefly::video