#include "document/ParticleGenerator.hpp"
#include "core/AutoPtr.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include <chrono>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <random>
using namespace firefly;
using namespace firefly::document;
constexpr static const float quadVec[] = {0.f,  0.0f, 0.0, 1.0f, 0.0f, 0.0,
                                          0.0f, 1.0f, 0.0, 0.0f, 1.0f, 0.0,
                                          1.0f, 1.0f, 0.0, 1.0f, 0.0f, 0.0};

constexpr static const float quadTex[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                                          0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};

constexpr static const uint32_t quadIndex[] = {0, 1, 2, 3, 4, 5};

ParticleGenerator::ParticleGenerator(uint32_t count) {
  _count = count;
  _generatorCount = 1;
  _lifetime = 10.0f;
  _particles.resize(count);
  for (auto i = 0; i < count; i++) {
    _particles[i].lifetime = 0.f;
  }
  _matrixModel = glm::mat4(1.0f);
  _localCoord = true;
  _direction = {0, 1};
  _speed = 1;
  _linearAcceleration = 0.0;
  _tangentialAcceleration = 0.0f;
  _radialAcceleration = 0.0f;
  _scale = {1, 1};
  _delay = 50;
  _randomAngle = 60;

  _position = {0, 0};
  _color = {1, 1, 1};
  _zIndex = 0;

  _shader = "particle";

  defineAttribute(ATTR_POSITION_X, &_current_position.x);
  defineAttribute(ATTR_POSITION_Y, &_current_position.y);
  defineAttribute(ATTR_LOCAL_COORD, &_localCoord);
  defineAttribute(ATTR_DIRECTION_X, &_direction.x);
  defineAttribute(ATTR_DIRECTION_Y, &_direction.y);
  defineAttribute(ATTR_SPEED, &_speed);
  defineAttribute(ATTR_LINEAR_ACCELERATION, &_linearAcceleration);
  defineAttribute(ATTR_TANGENTIAL_ACCELERATION, &_tangentialAcceleration);
  defineAttribute(ATTR_RADIAL_ACCELERATION, &_radialAcceleration);
  defineAttribute(ATTR_SCALE_X, &_scale.x);
  defineAttribute(ATTR_SCALE_Y, &_scale.y);
  defineAttribute(ATTR_DELAY, &_delay);
  defineAttribute(ATTR_RANDOM_ANGLE, &_randomAngle);
  defineAttribute(ATTR_GENERATE_COUNT, &_generatorCount);
  defineAttribute(ATTR_COLOR_R, &_color.r);
  defineAttribute(ATTR_COLOR_G, &_color.g);
  defineAttribute(ATTR_COLOR_B, &_color.b);
  defineAttribute(ATTR_ZINDEX, &_zIndex);
  defineAttribute(ATTR_TEXTURE, &_texture);
  defineAttribute(ATTR_LIFETIME, &_lifetime);
  defineAttribute(ATTR_SHADER, &_shader);

  _particleMatrixModels.resize(count);
  _particleMatrixTexcoords.resize(count);
  _particleColors.resize(count);
  _material = new video::Material();
  _material->setShader("particle");
  _material->setInstanced(_count);
  _material->setBlend(true);
  _material->setDepthTest(false);

  _geometry = new video::Geometry();
  _geometry->setAttribute(video::Geometry::ATTR_POSITION,
                          new video::Attribute(quadVec, 3));
  _geometry->setAttribute(video::Geometry::ATTR_TEXCOORD,
                          new video::Attribute(quadTex, 2));

  _attributeModels =
      new video::Attribute(sizeof(glm::mat4) * _count, 0, 16, false, true);
  _geometry->setAttribute(4, _attributeModels);
  _geometry->setVertexAttribDivisor(4);
  _geometry->setVertexAttribDivisor(4 + 1);
  _geometry->setVertexAttribDivisor(4 + 2);
  _geometry->setVertexAttribDivisor(4 + 3);

  _attributeTexcoords =
      new video::Attribute(sizeof(glm::mat4) * _count, 0, 16, false, true);
  _geometry->setAttribute(8, _attributeTexcoords);
  _geometry->setVertexAttribDivisor(8);
  _geometry->setVertexAttribDivisor(8 + 1);
  _geometry->setVertexAttribDivisor(8 + 2);
  _geometry->setVertexAttribDivisor(8 + 3);

  _attributeColors =
      new video::Attribute(sizeof(glm::vec3) * _count, 0, 3, false, true);
  _geometry->setAttribute(12, _attributeColors);
  _geometry->setVertexAttribDivisor(12);
}

void ParticleGenerator::setDirection(const glm::vec2 &direction) {
  beginAttrGroup(ATTR_DIRECTION);
  setAttribute("x", direction.x);
  setAttribute("y", direction.y);
  endAttrGroup();
}
void ParticleGenerator::setSpeed(float value) {
  setAttribute(ATTR_SPEED, value);
}
void ParticleGenerator::setLinearAcceleration(float value) {
  setAttribute(ATTR_LINEAR_ACCELERATION, value);
}
void ParticleGenerator::setTangentialAcceleration(float value) {
  setAttribute(ATTR_TANGENTIAL_ACCELERATION, value);
}
void ParticleGenerator::setRadialAcceleration(float value) {
  setAttribute(ATTR_RADIAL_ACCELERATION, value);
}
void ParticleGenerator::setSize(const glm::vec2 &value) {
  beginAttrGroup(ATTR_SCALE);
  setAttribute("x", value.x);
  setAttribute("y", value.y);
  endAttrGroup();
}
void ParticleGenerator::setPosition(const glm::vec2 &value) {
  beginAttrGroup(ATTR_POSITION);
  setAttribute("x", value.x);
  setAttribute("y", value.y);
  endAttrGroup();
}
void ParticleGenerator::setDelay(uint32_t delay) {
  setAttribute(ATTR_DELAY, delay);
}

void ParticleGenerator::setRandomAngle(uint32_t angle) {
  setAttribute(ATTR_RANDOM_ANGLE, angle);
}
void ParticleGenerator::setGeneratorCount(uint32_t count) {
  setAttribute(ATTR_GENERATE_COUNT, count);
}
void ParticleGenerator::setLocalCoord(bool value) {
  setAttribute(ATTR_LOCAL_COORD, value);
}

void ParticleGenerator::setColor(const glm::vec3 &color) {
  beginAttrGroup(ATTR_COLOR);
  setAttribute("r", color.r);
  setAttribute("g", color.g);
  setAttribute("b", color.b);
  endAttrGroup();
}
void ParticleGenerator::setLifetime(float value) {
  setAttribute(ATTR_LIFETIME, value);
}

void ParticleGenerator::setShader(const std::string &shader) {
  setAttribute(ATTR_SHADER, shader);
}

const glm::vec2 &ParticleGenerator::getDirection() const { return _direction; }

float ParticleGenerator::getSpeed() const { return _speed; }

float ParticleGenerator::getLinearAcceleration() const {
  return _linearAcceleration;
}

float ParticleGenerator::getTangentialAcceleration() const {
  return _tangentialAcceleration;
}

float ParticleGenerator::getRadialAcceleration() const {
  return _radialAcceleration;
}

const glm::vec2 &ParticleGenerator::getSize() const { return _scale; }

const glm::vec2 &ParticleGenerator::getPosition() const { return _position; }

uint32_t ParticleGenerator::getDelay() const { return _delay; }

uint32_t ParticleGenerator::getRandomAngle() const { return _randomAngle; }

uint32_t ParticleGenerator::getGeneratorCount() const {
  return _generatorCount;
}

const glm::vec3 &ParticleGenerator::getColor() const { return _color; }

float ParticleGenerator::getLifetime() const { return _lifetime; }

bool ParticleGenerator::isLocalCoord() const { return _localCoord; }

const std::string &ParticleGenerator::getShader() const { return _shader; }

void ParticleGenerator::setTexture(const std::string &path) {
  setAttribute(ATTR_TEXTURE, path);
}

void ParticleGenerator::setTexture(const core::AutoPtr<gl::Texture2D> &tex) {
  _material->setTexture(video::Material::DIFFUSE_TEX, tex);
  _texture = "";
}

const std::string &ParticleGenerator::getTexture() const { return _texture; }

void ParticleGenerator::generateParticle(uint32_t count) {
  auto current = 0;
  for (auto &p : _particles) {
    if (p.lifetime <= 0) {
      initParticle(p);
      current++;
    }
    if (current == count) {
      return;
    }
  }
}

void ParticleGenerator::initParticle(ParticleGenerator::Particle &p) {
  static auto zRay = glm::vec3(0, 0, 1);
  static std::random_device random;
  static std::mt19937 engine(random());
  p.lifetime = _lifetime;
  p.position = _position;
  p.speed = _speed * _direction;
  if (_randomAngle) {
    auto randomOffset =
        ((int32_t)(engine() % _randomAngle)) - (_randomAngle / 2.0f);
    if (randomOffset > 0) {
      p.speed = glm::rotate(glm::mat4(1.0f), glm::radians(randomOffset), zRay) *
                glm::vec4(p.speed, 0.0f, 1.0f);
    } else if (randomOffset < 0) {
      p.speed =
          glm::rotate(glm::mat4(1.0f), glm::radians(-randomOffset), -zRay) *
          glm::vec4(p.speed, 0.0f, 1.0f);
    }
  }
  p.color = _color;
  p.scale = _scale;
}
void ParticleGenerator::updateParticle(ParticleGenerator::Particle &p) {
  static auto zRay = glm::vec3(0, 0, 1);
  auto dis = glm::distance(_position, p.position);
  auto dir = glm::normalize(p.position - _position);
  auto vspeed = glm::normalize(glm::cross(glm::vec3(p.speed, 0.0f), zRay));
  p.speed += _linearAcceleration * _direction;
  p.speed += _tangentialAcceleration * glm::vec2(vspeed);
  if (dis > 0) {
    p.speed += _radialAcceleration * dir;
  }
  p.position += p.speed;
}
void ParticleGenerator::onTick() {
  using namespace std::chrono;
  static auto clock = system_clock::now();
  if (system_clock::now() - clock > _delay * 1ms) {
    auto count = 0;
    uint32_t generator = 0;
    for (auto &p : _particles) {
      if (generator < _generatorCount && p.lifetime <= 0) {
        initParticle(p);
        generator++;
      } else if (p.lifetime > 0) {
        updateParticle(p);
      }
      if (p.lifetime > 0) {
        _particleMatrixModels[count] =
            glm::translate(glm::mat4(1.0f), glm::vec3(p.position, _zIndex)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(p.scale, 1.0f));
        _particleMatrixTexcoords[count] = (1.0f);
        _particleColors[count] = p.color;
        count++;
        p.lifetime -= 0.1;
      }
    }
    _attributeModels->write(0, sizeof(glm::mat4) * count,
                            _particleMatrixModels.data());
    _attributeTexcoords->write(0, sizeof(glm::mat4) * count,
                               _particleMatrixTexcoords.data());
    _attributeColors->write(0, sizeof(glm::vec3) * count,
                            _particleColors.data());
    _material->setInstanced(count);
    clock = system_clock::now();
  }
  Renderable::onTick();
}

const core::AutoPtr<video::Geometry> &ParticleGenerator::getGeometry() const {
  return _geometry;
}
const core::AutoPtr<video::Material> &ParticleGenerator::getMaterial() const {
  return _material;
};
const glm::mat4 &ParticleGenerator::getMatrixModel() const {
  return _matrixModel;
};
void ParticleGenerator::onAttrChange(const std::string &name) {
  if (name == ATTR_TEXTURE) {
    _material->setTexture(video::Material::DIFFUSE_TEX, _texture);
  }
  if (name == ATTR_POSITION ||
      name.starts_with(std::string(ATTR_POSITION) + ".")) {
    if (_localCoord) {
      for (auto &p : _particles) {
        p.position += (_current_position - _position);
      }
    }
    _position = _current_position;
  }
  if (name == ATTR_SHADER) {
    _material->setShader(_shader);
  }
}