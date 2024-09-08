#pragma once
#include "Constant.hpp"
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "video/Image.hpp"
#include <glm/fwd.hpp>
#include <unordered_map>
namespace firefly::video {
class Material : public core::Object, public core::Cache<Material> {
private:
  std::string _type;
  std::unordered_map<std::string, std::string> _textures;

  glm::vec4 _ambient;
  glm::vec4 _diffuse;
  glm::vec4 _specular;
  glm::vec4 _emissive;
  glm::vec4 _reflective;
  glm::vec4 _transparent;

  bool _alphaTest;
  bool _depthTest;
  bool _stencilTest;

  bool _blend;
  bool _visible;

public:
  Material(const std::string &type);
  const std::string &getType() const;
  const std::unordered_map<std::string, std::string> &getTextures() const;
  void setTexture(const std::string &name, const std::string &texture);
  virtual void active(core::AutoPtr<Constant> &constants) const;

  const bool &isBlend() const;
  void setBlend(const bool &value);

  const bool &isVisible() const;
  void setVisible(const bool &value);

  const bool &isStencilTest() const;
  void setStencil(const bool &value);

  const bool &isDepthTest() const;
  void setDepthTest(const bool &value);

  const bool &isAlphaTest() const;
  void setAlphaTest(const bool &value);
};
} // namespace firefly::video