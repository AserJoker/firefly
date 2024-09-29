#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "Renderable.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Texture2D.hpp"
#include "video/Attribute.hpp"
#include <glm/fwd.hpp>
#include <vector>
namespace firefly::video {
class ParticleGenerator : public Renderable {
private:
  struct Particle {
    float lifetime;
    glm::vec3 speed;
    glm::vec3 scale;
    glm::vec3 position;
    glm::vec4 color;
  };

private:
  std::vector<Particle> _particles;

  core::AutoPtr<Attribute> _attributeModels;
  core::AutoPtr<Attribute> _attributeTexcoords;
  std::vector<glm::mat4> _particleMatrixModels;
  std::vector<glm::mat4> _particleMatrixTexcoords;

  core::AutoPtr<Geometry> _geometry;
  core::AutoPtr<Material> _material;
  glm::mat4 _matrixModel;
  uint32_t _count;

  bool _localCoords;
  glm::vec3 _initialVelocity;
  float _lifetime;
  float _linearAcceleration;

  glm::vec3 _position;
  glm::vec3 _initialScale;

protected:
  const core::AutoPtr<Geometry> &getGeometry() const override;
  const core::AutoPtr<Material> &getMaterial() const override;
  const glm::mat4 &getMatrixModel() const override;

public:
  ParticleGenerator(uint32_t count);
  void setTexture(
      const std::string &path, float blend = 1.0f,
      gl::TEXTURE_WRAP_MODE mappingmodeU = gl::TEXTURE_WRAP_MODE::REPEAT,
      gl::TEXTURE_WRAP_MODE mappingmodeV = gl::TEXTURE_WRAP_MODE::REPEAT);
  void setTexture(const core::AutoPtr<gl::Texture2D> &tex, float blend = 1.0f);
  const core::AutoPtr<gl::Texture2D> &getTexture() const;
  void onTick() override;
};
} // namespace firefly::video