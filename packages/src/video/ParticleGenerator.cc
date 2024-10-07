#include "video/ParticleGenerator.hpp"
#include "core/AutoPtr.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include <chrono>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
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
  _lifetime = 1.0f;
  _particles.resize(count);
  for (auto i = 0; i < count; i++) {
    _particles[i].lifetime = 0.f;
  }
  _matrixModel = glm::mat4(1.0f);
  _localCoords = true;
  _initialDirection = {0, 0.2, 0};
  _initialVelocity = 0.1;
  _linearAcceleration = 0.1;
  _tangentialAcceleration = 0.0f;
  _radialAcceleration = 0.f;
  _initialScale = {1, 1, 0};

  _position = {100, 100, 0};

  _particleMatrixModels.resize(count);
  _particleMatrixTexcoords.resize(count);
  _material = new Material();
  _material->setShader("particle");
  _material->setInstanced(_count);
  _material->setBlend(true);
  _material->setDepthTest(false);

  _geometry = new Geometry();
  _geometry->setAttribute(Geometry::ATTR_POSITION, new Attribute(quadVec, 3));
  _geometry->setAttribute(Geometry::ATTR_TEXCOORD, new Attribute(quadTex, 2));

  _attributeModels =
      new Attribute(sizeof(glm::mat4) * _count, 0, 16, false, true);
  _geometry->setAttribute(4, _attributeModels);
  _geometry->setVertexAttribDivisor(4);
  _geometry->setVertexAttribDivisor(4 + 1);
  _geometry->setVertexAttribDivisor(4 + 2);
  _geometry->setVertexAttribDivisor(4 + 3);

  _attributeTexcoords =
      new Attribute(sizeof(glm::mat4) * _count, 0, 16, false, true);
  _geometry->setAttribute(8, _attributeTexcoords);
  _geometry->setVertexAttribDivisor(8 + 1);
  _geometry->setVertexAttribDivisor(8 + 2);
  _geometry->setVertexAttribDivisor(8 + 3);
  _geometry->setVertexAttribDivisor(8 + 4);
}

void ParticleGenerator::setDirection(const glm::vec3 &direction) {
  _initialDirection = direction;
}
void ParticleGenerator::setInitialVelocity(float value) {
  _initialVelocity = value;
}
void ParticleGenerator::setLinearAcceleration(float value) {
  _linearAcceleration = value;
}
void ParticleGenerator::setTangentialAcceleration(float value) {
  _tangentialAcceleration = value;
}
void ParticleGenerator::setRadialAcceleration(float value) {
  _radialAcceleration = value;
}
void ParticleGenerator::setScale(const glm::vec3 &value) {
  _initialScale = value;
}
void ParticleGenerator::setPosition(const glm::vec3 &value) {
  _position = value;
}

void ParticleGenerator::onTick() {
  using namespace std::chrono;
  static auto clock = system_clock::now();
  static auto zRay = glm::vec3(0, 0, 1);
  if (system_clock::now() - clock > 50ms) {
    auto count = 0;
    bool generator = false;
    static uint32_t fcount = 0;
    for (auto &p : _particles) {
      if (!generator && p.lifetime <= 0 && fcount % 10 == 0) {
        p.lifetime = _lifetime;
        p.position = _position;
        p.speed = _initialVelocity * _initialDirection;
        auto vspeed = glm::normalize(glm::cross(p.speed, zRay));
        p.color = {1.0f, 1.0f, 1.0f, 1.0f};
        p.scale = _initialScale;
        generator = true;
      } else if (p.lifetime > 0) {
        auto dis = glm::distance(_position, p.position);
        auto vspeed = glm::normalize(glm::cross(p.speed, zRay));
        auto acceleration = 1 + _linearAcceleration;
        if (dis != 0) {
          acceleration += _radialAcceleration / dis;
        }
        p.speed *= acceleration;
        p.speed += _tangentialAcceleration * vspeed;
        p.position += p.speed;
      }
      if (p.lifetime > 0) {
        _particleMatrixModels[count] =
            glm::translate(glm::mat4(1.0f), p.position) *
            glm::scale(glm::mat4(1.0f), p.scale);
        _particleMatrixTexcoords[count] = (1.0f);
        count++;
      }
      if (fcount % 10 == 0) {
        p.lifetime -= 0.1;
      }
    }
    fcount++;
    _attributeModels->write(0, sizeof(glm::mat4) * count,
                            _particleMatrixModels.data());
    _attributeTexcoords->write(0, sizeof(glm::mat4) * count,
                               _particleMatrixTexcoords.data());
    _material->setInstanced(count);
    clock = system_clock::now();
  }
  Renderable::onTick();
}

void ParticleGenerator::setTexture(const std::string &path, float blend,
                                   gl::TEXTURE_WRAP_MODE mappingmodeU,
                                   gl::TEXTURE_WRAP_MODE mappingmodeV) {
  _material->setTexture(Material::DIFFUSE_TEX, path, glm::mat4(1.0f), blend,
                        mappingmodeU, mappingmodeV);
}

void ParticleGenerator::setTexture(const core::AutoPtr<gl::Texture2D> &tex,
                                   float blend) {
  _material->setTexture(Material::DIFFUSE_TEX, tex, blend);
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