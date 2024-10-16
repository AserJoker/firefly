#pragma once
#include "core/AutoPtr.hpp"
#include "document/Renderable.hpp"
#include "gl/Texture2D.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include <glm/fwd.hpp>
#include <vector>

namespace firefly::document {
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

  core::AutoPtr<video::Attribute> _attributeModels;
  core::AutoPtr<video::Attribute> _attributeTexcoords;
  core::AutoPtr<video::Attribute> _attributeColors;
  std::vector<glm::mat4> _particleMatrixModels;
  std::vector<glm::mat4> _particleMatrixTexcoords;
  std::vector<glm::vec3> _particleColors;

  core::AutoPtr<video::Geometry> _geometry;
  core::AutoPtr<video::Material> _material;

  glm::ivec4 _bindingRect;

  glm::mat4 _matrixModel;
  uint32_t _count;
  glm::vec3 _color;

  bool _localCoord;
  glm::vec2 _direction;
  float _lifetime;
  float _speed;
  float _linearAcceleration;
  float _tangentialAcceleration;
  float _radialAcceleration;
  uint32_t _delay;
  uint32_t _generatorCount;
  uint32_t _randomAngle;
  uint32_t _zIndex;

  glm::vec2 _position;
  glm::vec2 _current_position;
  glm::vec2 _scale;
  std::string _texture;
  std::string _shader;

private:
  void initParticle(Particle &p);
  void updateParticle(Particle &p);

protected:
  const core::AutoPtr<video::Geometry> &getGeometry() const override;
  const core::AutoPtr<video::Material> &getMaterial() const override;
  const glm::mat4 &getMatrixModel() const override;
  const glm::ivec4 &getBindingRect() const override;

  void onAttrChange(const std::string &name) override;

public:
  ParticleGenerator(uint32_t count);
  void setTexture(const core::AutoPtr<gl::Texture2D> &tex);
  void setTexture(const std::string &path);
  const std::string &getTexture() const;

  void setDirection(const glm::vec2 &direction);
  void setSpeed(float value);
  void setLinearAcceleration(float value);
  void setTangentialAcceleration(float value);
  void setRadialAcceleration(float value);
  void setSize(const glm::vec2 &value);
  void setPosition(const glm::vec2 &value);
  void setDelay(uint32_t delay);
  void setRandomAngle(uint32_t angle);
  void setGeneratorCount(uint32_t count);
  void setLocalCoord(bool value);
  void setColor(const glm::vec3 &color);
  void setLifetime(float value);
  void setShader(const std::string &shader);

  const glm::vec2 &getDirection() const;
  float getSpeed() const;
  float getLinearAcceleration() const;
  float getTangentialAcceleration() const;
  float getRadialAcceleration() const;
  const glm::vec2 &getSize() const;
  const glm::vec2 &getPosition() const;
  uint32_t getDelay() const;
  uint32_t getRandomAngle() const;
  uint32_t getGeneratorCount() const;
  bool isLocalCoord() const;
  const glm::vec3 &getColor() const;
  float getLifetime() const;
  const std::string &getShader() const;

  void generateParticle(uint32_t count = 1);
  void onTick() override;

public:
  static constexpr inline const char *ATTR_POSITION = "position";
  static constexpr inline const char *ATTR_POSITION_X = "position.x";
  static constexpr inline const char *ATTR_POSITION_Y = "position.y";
  static constexpr inline const char *ATTR_LOCAL_COORD = "localCoord";
  static constexpr inline const char *ATTR_DIRECTION = "direction";
  static constexpr inline const char *ATTR_DIRECTION_X = "direction.x";
  static constexpr inline const char *ATTR_DIRECTION_Y = "direction.y";
  static constexpr inline const char *ATTR_SPEED = "speed";
  static constexpr inline const char *ATTR_LINEAR_ACCELERATION =
      "linearAcceleration";
  static constexpr inline const char *ATTR_TANGENTIAL_ACCELERATION =
      "tangentialAcceleration";
  static constexpr inline const char *ATTR_RADIAL_ACCELERATION =
      "radialAcceleration";
  static constexpr inline const char *ATTR_SCALE = "scale";
  static constexpr inline const char *ATTR_SCALE_X = "scale.x";
  static constexpr inline const char *ATTR_SCALE_Y = "scale.y";
  static constexpr inline const char *ATTR_DELAY = "delay";
  static constexpr inline const char *ATTR_RANDOM_ANGLE = "randomAngle";
  static constexpr inline const char *ATTR_GENERATE_COUNT = "generateCount";
  static constexpr inline const char *ATTR_COLOR = "color";
  static constexpr inline const char *ATTR_COLOR_R = "color.r";
  static constexpr inline const char *ATTR_COLOR_G = "color.g";
  static constexpr inline const char *ATTR_COLOR_B = "color.b";
  static constexpr inline const char *ATTR_ZINDEX = "zIndex";
  static constexpr inline const char *ATTR_TEXTURE = "texture";
  static constexpr inline const char *ATTR_LIFETIME = "lifetime";
  static constexpr inline const char *ATTR_SHADER = "shader";
};
} // namespace firefly::document