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
    glm::vec2 speed;
    glm::vec2 scale;
    glm::vec2 position;
    glm::vec3 color;
  };

private:
  std::vector<Particle> _particles;

  core::AutoPtr<Attribute> _attributeModels;
  core::AutoPtr<Attribute> _attributeTexcoords;
  core::AutoPtr<Attribute> _attributeColors;
  std::vector<glm::mat4> _particleMatrixModels;
  std::vector<glm::mat4> _particleMatrixTexcoords;
  std::vector<glm::vec3> _particleColors;

  core::AutoPtr<Geometry> _geometry;
  core::AutoPtr<Material> _material;
  glm::mat4 _matrixModel;
  uint32_t _count;
  glm::vec3 _color;

  bool _localCoords;
  glm::vec2 _direction;
  float _lifetime;
  float _speed;
  float _linearAcceleration;
  float _tangentialAcceleration;
  float _radialAcceleration;
  uint32_t _delay;
  uint32_t _generatorCount;
  uint32_t _initialRandomAngle;
  uint32_t _zIndex;

  glm::vec2 _position;
  glm::vec2 _current_position;
  glm::vec2 _scale;
  std::string _texture;

private:
  void generateParticle(Particle &p);
  void updateParticle(Particle &p);

protected:
  const core::AutoPtr<Geometry> &getGeometry() const override;
  const core::AutoPtr<Material> &getMaterial() const override;
  const glm::mat4 &getMatrixModel() const override;

  void onAttrChange(const std::string &name) override;

public:
  ParticleGenerator(uint32_t count);
  void setTexture(const std::string &path);
  void setTexture(const core::AutoPtr<gl::Texture2D> &tex);
  const core::AutoPtr<gl::Texture2D> &getTexture() const;

  void setDirection(const glm::vec2 &direction);
  void setSpeed(float value);
  void setLinearAcceleration(float value);
  void setTangentialAcceleration(float value);
  void setRadialAcceleration(float value);
  void setScale(const glm::vec2 &value);
  void setPosition(const glm::vec2 &value);
  void setDelay(uint32_t delay);
  void setRandomAngle(uint32_t angle);
  void setGeneratorCount(uint32_t count);
  void setLocalCoord(bool value);
  void setColor(const glm::vec3 &color);

  const glm::vec2 &getDirection() const;
  float getSpeed() const;
  float getLinearAcceleration() const;
  float getTangentialAcceleration() const;
  float getRadialAcceleration() const;
  const glm::vec2 &getScale() const;
  const glm::vec2 &getPosition() const;
  uint32_t getDelay() const;
  uint32_t getRandomAngle() const;
  uint32_t getGeneratorCount() const;
  bool isLocalCoord() const;
  const glm::vec3 &getColor() const;

  void generateParticle();
  void onTick() override;
};
} // namespace firefly::video