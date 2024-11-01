#include "document/ParticleGenerator.hpp"
#include "core/AutoPtr.hpp"
#include "core/Math.hpp"
#include "document/Renderable.hpp"
#include "internal/geometry.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Texture.hpp"
#include <chrono>
#include <glm/geometric.hpp>
using namespace std::chrono;
using namespace firefly;
using namespace firefly::document;
ParticleGenerator::ParticleGenerator()
    : _shader("particle"), _maxCount(10), _size(11), _count(0), _lifetime(100),
      _linerAcceleration(0), _tangentialAcceleration(0),
      _position(100.0f, 100.0f), _velocity(0.0f, 2.0f),
      _partices(nullptr, nullptr, nullptr), _matrix(1.0f) {}

const core::AutoPtr<video::Material> &ParticleGenerator::getMaterial() const {
  return _material;
}

const core::AutoPtr<video::Geometry> &ParticleGenerator::getGeometry() const {
  return _geometry;
}

const glm::mat4 &ParticleGenerator::getMatrix() const { return _matrix; }

void ParticleGenerator::onLoad() {
  _geometry = new video::Geometry();
  _material = new video::Material();
  _material->setDepthTest(false);
  _material->setIsTransparent(true);
  _material->setShader(_shader);

  core::AutoPtr attrPosition = new video::Attribute(internal::rectPosition, 3);
  core::AutoPtr attrCoord = new video::Attribute(internal::rectTextureCoord, 2);
  core::AutoPtr attrIndices = new video::AttributeIndex(internal::rectIndices);

  _geometry->setAttribute(0, attrPosition);
  _geometry->setAttribute(1, attrCoord);
  _geometry->setAttributeIndex(attrIndices);

  _attrInstanceModel =
      new video::Attribute(sizeof(glm::mat4) * _maxCount, nullptr, 16);

  _geometry->setAttribute(2, _attrInstanceModel);
  _geometry->setVertexAttribDivisor(2);
  _geometry->setVertexAttribDivisor(3);
  _geometry->setVertexAttribDivisor(4);
  _geometry->setVertexAttribDivisor(5);

  _material->setTexture("diffuse", new video::Texture("star.png"));
  Renderable::onLoad();
}

void ParticleGenerator::onUnload() {
  Renderable::onUnload();
  while (_partices._list && _partices._list->next != _partices._list) {
    auto next = _partices._list->next;
    _partices._list->next = next->next;
    delete next;
    _count--;
  }
  delete _partices._list;
  _partices._start = nullptr;
  _partices._end = nullptr;
  _partices._list = nullptr;
}

void ParticleGenerator::onTick() {
  static auto old = system_clock::now();
  auto now = system_clock::now();
  if (now - old < 50ms) {
    Renderable::onTick();
    return;
  }
  old = now;
  _matrixs.clear();
  Particle *p = _partices._start;
  auto count = _count;
  while (count) {
    updateParticle(p);
    p = p->next;
    count--;
  }
  if (_count < _maxCount) {
    static auto old = system_clock::now();
    auto now = system_clock::now();
    if (now - old > 200ms) {
      createParticle();
      old = now;
    }
  }
  if (!_partices._start) {
    Node::onTick();
  } else {
    _attrInstanceModel->write(0, _count * sizeof(glm::mat4), _matrixs.data());
    _material->setInstanced(_count);
    Renderable::onTick();
  }
}

void ParticleGenerator::createParticle() {
  Particle *p = nullptr;
  if (!_partices._list) {
    p = new Particle();
    _partices._list = p;
    _partices._start = p;
    _partices._end = p;
    p->next = p;
  } else if (!_partices._start) {
    p = _partices._list;
    _partices._start = p;
    _partices._end = p;
  } else if (_partices._end->next != _partices._start) {
    _partices._end = _partices._end->next;
    p = _partices._end;
  } else {
    p = new Particle();
    p->next = _partices._start;
    _partices._end->next = p;
    _partices._end = p;
  }
  initParticle(p);
  _count++;
}

void ParticleGenerator::updateParticle(ParticleGenerator::Particle *particle) {
  particle->lifetime--;
  if (!particle->lifetime) {
    _count--;
    _partices._start = particle->next;
    if (_partices._start->lifetime <= 0) {
      _partices._start = nullptr;
      _partices._end = nullptr;
    }
    return;
  }
  static auto zDirection = glm::vec3(0, 0, 1);
  auto v = glm::vec3{particle->velocity.x, particle->velocity.y, 0.0};
  auto vlenght = glm::length(v);
  glm::vec3 tv(0.0);
  if (v.x != 0 || v.y != 0) {
    tv = glm::normalize(glm::cross(v, zDirection)) * vlenght *
         _tangentialAcceleration;
  }

  particle->velocity *= (1.0 + _linerAcceleration);
  particle->velocity += {tv.x, tv.y, 0.0};
  particle->position += particle->velocity;

  _matrixs.pushBack(
      core::translate({particle->position.x, particle->position.y, 0.0f}) *
      core::scale({particle->size * 1.0f, particle->size * 1.0f, 1.0}));
}

void ParticleGenerator::initParticle(ParticleGenerator::Particle *particle) {
  particle->lifetime = _lifetime;
  particle->position = _position;
  particle->velocity = _velocity;
  particle->size = _size;
  _matrixs.pushBack(
      core::translate({particle->position.x, particle->position.y, 0.0f}) *
      core::scale({particle->size * 1.0f, particle->size * 1.0f, 1.0}));
}