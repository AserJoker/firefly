#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Point.hpp"
#include "document/Renderable.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
namespace firefly::document {
class ParticleGenerator
    : public Renderable,
      public Node::Register<"particle-generator", ParticleGenerator> {
private:
  struct Particle;
  struct Particle {
    uint32_t lifetime;
    uint32_t size;
    core::Point<float> position;
    core::Point<float> velocity;
    Particle *next;
  };

private:
  std::string _shader;
  uint32_t _maxCount;
  uint32_t _size;
  uint32_t _count;
  uint32_t _lifetime;
  float _linerAcceleration;
  float _tangentialAcceleration;
  core::Point<float> _position;
  core::Point<float> _velocity;

  struct {
    Particle *_start;
    Particle *_end;
    Particle *_list;
  } _partices;

private:
  core::Array<glm::mat4> _matrixs;
  core::AutoPtr<video::Attribute> _attrInstanceModel;
  core::AutoPtr<video::Material> _material;
  core::AutoPtr<video::Geometry> _geometry;
  glm::mat4 _matrix;

private:
  void createParticle();
  void updateParticle(Particle *particle);
  void initParticle(Particle *particle);

protected:
  const core::AutoPtr<video::Material> &getMaterial() const override;
  const core::AutoPtr<video::Geometry> &getGeometry() const override;
  const glm::mat4 &getMatrix() const override;

protected:
  void onMaxCountChange();
  void onShaderChange();
  void onSizeChange();
  void onLoad() override;
  void onUnload() override;
  void onTick() override;

public:
  ParticleGenerator();

public:
  static inline constexpr auto PROP_SHADER = "shader";
  static inline constexpr auto PROP_MAX_COUNT = "max-count";
  static inline constexpr auto PROP_SIZE = "size";
};
} // namespace firefly::document