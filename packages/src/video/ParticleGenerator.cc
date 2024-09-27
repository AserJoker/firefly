#include "video/ParticleGenerator.hpp"
#include "core/AutoPtr.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
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
    _particles[i].color = {1, 1, 1, 1};
    _particles[i].position = {0, 0, 0};
    _particles[i].scale = {1.0f, 1.0f, 1.0f};
    _particles[i].speed = {0.0f, 0.0f, 0.0f};
  }
  _matrixModel = glm::mat4(1.0f);
  _random = false;
  _localCoords = true;
  _spread = 0.f;
  _flatness = 0.f;
  _initialVelocity = {0, 1, 0};
  _angularVelocity = 0.f;
  _spinVelocity = 0.f;
  _linerAcceleration = 0.f;
  _radialAcceleration = 0.f;
  _tangentialAcceleration = 0.f;
  _damping = {0, 0, 0};
  _scale = {1.0f, 1.0f, 1.0f};

  _position = {0, 0, 0};

  _particleMatrixModels.resize(count);
  _particleMatrixTexcoords.resize(count);
  _material = new Material();
  _material->setShader("particle");
  _material->setInstanced(_count);

  _geometry = new Geometry();
  _geometry->setAttribute(Geometry::ATTR_POSITION, new Attribute(quadVec, 3));
  _geometry->setAttribute(Geometry::ATTR_TEXCOORD, new Attribute(quadTex, 2));

  core::AutoPtr<Attribute> model = new Attribute(
      sizeof(glm::mat4) * _count, 0, typeid(float), 16, false, true);
  _geometry->setAttribute(Geometry::ATTR_CUSTOM, model);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM + 1);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM + 2);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM + 3);

  core::AutoPtr<Attribute> texModel = new Attribute(
      sizeof(glm::mat4) * _count, 0, typeid(float), 16, false, true);
  _geometry->setAttribute(Geometry::ATTR_CUSTOM + 4, texModel);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM + 4);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM + 5);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM + 6);
  _geometry->setVertexAttribDivisor(Geometry::ATTR_CUSTOM + 7);
}
void ParticleGenerator::onTick() {
  auto count = 0;
  bool generator = false;
  for (auto &p : _particles) {
    if (!generator && p.lifetime <= 0) {
      p.lifetime = _lifetime;
      p.position = _position;
      p.speed = _initialVelocity;
      p.color = {1.0f, 1.0f, 1.0f, 1.0f};
      p.scale = {1.0f, 1.0f, 1.0f};
      generator = true;
    } else {
      auto d = glm::distance(p.position, _position);
      auto vspeed = glm::cross(p.speed, {0, 0, 1});
      p.speed += vspeed * _tangentialAcceleration;
      p.speed *= _radialAcceleration * d;
      p.speed *= _linerAcceleration;
      p.speed -= _damping * d;
      p.position += p.speed;
      p.scale += _scale;
    }
    if (p.lifetime > 0) {
      _particleMatrixModels[count] =
          (glm::scale(glm::mat4(1.0f), p.scale) *
           glm::translate(glm::mat4(1.0f), p.position));
      _particleMatrixTexcoords[count] = (1.0f);
      count++;
    }
    p.lifetime -= 0.1;
  }
  _attributeModels->write(0, sizeof(glm::mat4) * count,
                          _particleMatrixModels.data());
  _attributeTexcoords->write(0, sizeof(glm::mat4) * count,
                             _particleMatrixTexcoords.data());
  _material->setInstanced(count);
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