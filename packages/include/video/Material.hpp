#pragma once
#include "Constant.hpp"
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Image.hpp"
#include <unordered_map>
namespace firefly::video {
class Material : public core::Object {
private:
  core::AutoPtr<Shader> _shader;
  std::string _type;
  std::unordered_map<std::string, core::AutoPtr<Image>> _textures;
  bool _alphaTest;
  bool _depthTest;
  bool _stencilTest;
  bool _blend;
  bool _visible;

public:
  Material(const std::string &type);
  const std::string &getType() const;
  const std::unordered_map<std::string, core::AutoPtr<Image>> &
  getTextures() const;
  void setTexture(const std::string &name, const core::AutoPtr<Image> &texture);
  void setShader(const core::AutoPtr<Shader> &shader);
  virtual void active(core::AutoPtr<Constant> &constants) const;
  const core::AutoPtr<Shader> &getShader() const;
};
} // namespace firefly::video