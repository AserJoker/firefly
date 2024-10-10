#include "video/ParticleGenerator.hpp"
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
using namespace firefly::video;
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
  _localCoords = true;
  _direction = {0, 1};
  _speed = 1;
  _linearAcceleration = 0.0;
  _tangentialAcceleration = 0.0f;
  _radialAcceleration = 0.0f;
  _scale = {1, 1};
  _delay = 50;
  _initialRandomAngle = 60;

  _position = {0, 0};
  _color = {1, 1, 1};
  _zIndex = 0;

  defineAttribute("position.x", &_position.x);
  defineAttribute("position.y", &_position.y);
  defineAttribute("localCoords", &_localCoords);
  defineAttribute("direction.x", &_direction.x);
  defineAttribute("direction.y", &_direction.y);
  defineAttribute("speed", &_speed);
  defineAttribute("linearAcceleration", &_linearAcceleration);
  defineAttribute("tangentialAcceleration", &_tangentialAcceleration);
  defineAttribute("radialAcceleration", &_radialAcceleration);
  defineAttribute("scale.x", &_scale.x);
  defineAttribute("scale.y", &_scale.y);
  defineAttribute("delay", &_delay);
  defineAttribute("randomAngle", &_initialRandomAngle);
  defineAttribute("generateCount", &_generatorCount);
  defineAttribute("color.r", &_color.r);
  defineAttribute("color.g", &_color.g);
  defineAttribute("color.b", &_color.b);
  defineAttribute("zIndex", &_zIndex);
  defineAttribute("texture", &_texture);

  _particleMatrixModels.resize(count);
  _particleMatrixTexcoords.resize(count);
  _particleColors.resize(count);
  _material = new Material();
  _material->setShader("particle");
  _material->setInstanced(_count);
  _material->setBlend(true);
  _material->setDepthTest(false);

  _geometry = new Geometry();
  _geometry->setAttribute(Geometry::ATTR_POSITION,
                          new video::Attribute(quadVec, 3));
  _geometry->setAttribute(Geometry::ATTR_TEXCOORD,
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
  _direction = direction;
}
void ParticleGenerator::setSpeed(float value) { _speed = value; }
void ParticleGenerator::setLinearAcceleration(float value) {
  _linearAcceleration = value;
}
void ParticleGenerator::setTangentialAcceleration(float value) {
  _tangentialAcceleration = value;
}
void ParticleGenerator::setRadialAcceleration(float value) {
  _radialAcceleration = value;
}
void ParticleGenerator::setScale(const glm::vec2 &value) { _scale = value; }
void ParticleGenerator::setPosition(const glm::vec2 &value) {
  if (_localCoords) {
    for (auto &p : _particles) {
      p.position += (value - _position);
    }
  }
  _position = value;
}
void ParticleGenerator::setDelay(uint32_t delay) { _delay = delay; }

void ParticleGenerator::setRandomAngle(uint32_t angle) {
  _initialRandomAngle = angle;
}
void ParticleGenerator::setGeneratorCount(uint32_t count) {
  _generatorCount = count;
}
void ParticleGenerator::setLocalCoord(bool value) { _localCoords = value; }

void ParticleGenerator::setColor(const glm::vec3 &color) { _color = color; }

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

const glm::vec2 &ParticleGenerator::getScale() const { return _scale; }

const glm::vec2 &ParticleGenerator::getPosition() const { return _position; }

uint32_t ParticleGenerator::getDelay() const { return _delay; }

uint32_t ParticleGenerator::getRandomAngle() const {
  return _initialRandomAngle;
}

uint32_t ParticleGenerator::getGeneratorCount() const {
  return _generatorCount;
}

const glm::vec3 &ParticleGenerator::getColor() const { return _color; }

bool ParticleGenerator::isLocalCoord() const { return _localCoords; }

void ParticleGenerator::generateParticle() {
  for (auto &p : _particles) {
    if (p.lifetime <= 0) {
      generateParticle(p);
      return;
    }
  }
}

void ParticleGenerator::generateParticle(ParticleGenerator::Particle &p) {
  static auto zRay = glm::vec3(0, 0, 1);
  static std::random_device random;
  static std::mt19937 engine(random());
  p.lifetime = _lifetime;
  p.position = _position;
  p.speed = _speed * _direction;
  if (_initialRandomAngle) {
    auto randomOffset = ((int32_t)(engine() % _initialRandomAngle)) -
                        (_initialRandomAngle / 2.0f);
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
        generateParticle(p);
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

void ParticleGenerator::setTexture(const std::string &path) {
  setAttribute("texture", path);
}

void ParticleGenerator::setTexture(const core::AutoPtr<gl::Texture2D> &tex) {
  _material->setTexture(Material::DIFFUSE_TEX, tex);
}

const core::AutoPtr<gl::Texture2D> &ParticleGenerator::getTexture() const {
  return _material->getTextures().at(Material::DIFFUSE_TEX).texture;
}
const core::AutoPtr<Geometry> &ParticleGenerator::getGeometry() const {
  return _geometry;
}
const core::AutoPtr<Material> &ParticleGenerator::getMaterial() const {
  return _material;
};
const glm::mat4 &ParticleGenerator::getMatrixModel() const {
  return _matrixModel;
};
void ParticleGenerator::onAttrChange(const std::string &name) {
  if (name == "texture") {
    _material->setTexture(Material::DIFFUSE_TEX, _texture);
  }
}