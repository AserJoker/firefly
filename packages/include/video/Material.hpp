#pragma once
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Constant.hpp"
#include "gl/Texture2D.hpp"
#include "gl/TextureWrapMode.hpp"
#include "gl/AlphaFunc.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <set>
#include <unordered_map>

namespace firefly::video {
class Material : public core::Object, public core::Cache<Material> {
public:
  struct TextureInfo {
    core::AutoPtr<gl::Texture2D> texture;
    float blend;
    glm::mat4 textureCoordMatrix;
  };

  using Attribute = std::function<void(core::AutoPtr<gl::Constant>)>;
  inline static constexpr auto AMBIENT_COLOR = "ambient_color";
  inline static constexpr auto DIFFUSE_COLOR = "diffuse_color";
  inline static constexpr auto SPECULAR_COLOR = "specular_color";
  inline static constexpr auto EMISSIVE_COLOR = "emissive_color";
  inline static constexpr auto REFLECTIVE_COLOR = "reflective_color";
  inline static constexpr auto TRANSPARENT_COLOR = "transparent_color";
  inline static constexpr auto REFLECTIVITY = "reflectivity";
  inline static constexpr auto OPACITY = "opacity";
  inline static constexpr auto SHININESS = "shininess";
  inline static constexpr auto SHININESS_STRENGTH = "shininess_strength";
  inline static constexpr auto DIFFUSE_TEX = "diffuse_texture";
  inline static constexpr auto SPECULAR_TEX = "specular_texture";
  inline static constexpr auto AMBIENT_TEX = "ambient_texture";
  inline static constexpr auto EMISSIVE_TEX = "emissive_texture";
  inline static constexpr auto HEIGHT_TEX = "height_texture";
  inline static constexpr auto NORMALS_TEX = "normals_texture";
  inline static constexpr auto SHININESS_TEX = "shininess_texture";
  inline static constexpr auto OPACITY_TEX = "opacity_texture";
  inline static constexpr auto DISPLACEMENT_TEX = "displacement_texture";
  inline static constexpr auto LIGHTMAP_TEX = "lightmap_texture";
  inline static constexpr auto REFLECTION_TEX = "reflection_texture";
  inline static constexpr auto BASE_COLOR_TEX = "base_color_texture";
  inline static constexpr auto NORMAL_CAMERA_TEX = "normal_camera_texture";
  inline static constexpr auto EMISSION_COLOR_TEX = "emission_color_texture";
  inline static constexpr auto METALNESS_TEX = "metalness_texture";
  inline static constexpr auto DIFFUSE_ROUGHNESS_TEX =
      "diffuse_roughness_texture";
  inline static constexpr auto AMBIENT_OCCLUSION_TEX =
      "ambient_occlusion_texture";
  inline static constexpr auto SHEEN_TEX = "sheen_texture";
  inline static constexpr auto CLEARCOAT_TEX = "clearcoat_texture";
  inline static constexpr auto TRANSMISSION_TEX = "transmission_texture";

private:
  std::unordered_map<std::string, TextureInfo> _textures;
  std::unordered_map<std::string, Attribute> _attributes;

  std::set<std::string> _enableAttributes;

  std::string _name;

  glm::vec3 _ambient;
  glm::vec3 _diffuse;
  glm::vec3 _specular;
  glm::vec3 _emissive;
  glm::vec3 _reflective;
  glm::vec3 _transparent;
  float _reflectivity;
  bool _wireframe;
  bool _cullBackface;
  bool _blendAdd;
  float _opacity;
  float _shininess;
  float _shininessStrength;

  bool _alphaTest;
  std::pair<gl::ALPHA_FUNC, float> _alphaFunc;
  bool _depthTest;
  bool _stencilTest;

  bool _blend;
  std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> _blendFunc;
  bool _visible;

public:
  Material();
  const std::unordered_map<std::string, TextureInfo> &getTextures() const;
  std::unordered_map<std::string, TextureInfo> &getTextures();
  void setTexture(
      const std::string &name, const std::string &path,
      const glm::mat4 &textureCoordMatrix = glm::mat4(1.0f), float blend = 1.0f,
      gl::TEXTURE_WRAP_MODE mappingmodeU = gl::TEXTURE_WRAP_MODE::REPEAT,
      gl::TEXTURE_WRAP_MODE mappingmodeV = gl::TEXTURE_WRAP_MODE::REPEAT);
  void setTexture(const std::string &name,
                  const core::AutoPtr<gl::Texture2D> &tex,
                  const glm::mat4 &textureCoordMatrix = glm::mat4(1.0f),
                  float blend = 1.0f);

  const glm::vec3 &getAmbient() const;
  const glm::vec3 &getDiffuse() const;
  const glm::vec3 &getSpecular() const;
  const glm::vec3 &getEmissive() const;
  const glm::vec3 &getReflective() const;
  const glm::vec3 &getTransparent() const;
  const float &getReflectivity() const;
  const bool &isWireframe() const;
  const bool &isCullBackface() const;
  const bool &isBlendAdd() const;
  const float &getOpacity() const;
  const float &getShininess() const;
  const float &getShininessStrength() const;
  const std::string &getName() const;

  void setAmbient(const glm::vec3 &color);
  void setDiffuse(const glm::vec3 &color);
  void setSpecular(const glm::vec3 &color);
  void setEmissive(const glm::vec3 &color);
  void setReflective(const glm::vec3 &color);
  void setTransparent(const glm::vec3 &color);
  void setReflectivity(float value);
  void setIsWireframe(bool value);
  void setIsCullBackface(bool value);
  void setIsBlendAdd(bool value);
  void setOpacity(float value);
  void setShininess(float value);
  void setShininessStrength(float value);
  void setName(const std::string &name);

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
  const std::pair<gl::ALPHA_FUNC, float>& getAlphaFunc() const;
  void setAlphaFunc(const std::pair<gl::ALPHA_FUNC, float>& func);

  void enableAttribute(const std::string &name);
  void disableAttribute(const std::string &name);

  const std::unordered_map<std::string, Attribute> &getAttributes() const;
};
} // namespace firefly::video