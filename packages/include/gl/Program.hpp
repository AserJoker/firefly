#pragma once
#include "Uniform.hpp"
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "gl/Shader.hpp"
#include <glm/glm.hpp>

namespace firefly::gl {
class Program : public core::Object, public core::Cache<Program> {
private:
  core::Unsigned_t _handle;
  core::Map<core::String_t, core::Unsigned_t> _locations;
  core::Map<core::String_t, Uniform> _uniforms;

private:
  const core::Unsigned_t &getUniformLocation(const core::String_t &name);
  void setUniform(const core::String_t &name, core::Integer_t value);
  void setUniform(const core::String_t &name, core::Boolean_t value);
  void setUniform(const core::String_t &name, core::Float_t value);
  void setUniform(const core::String_t &name, const glm::vec2 &value);
  void setUniform(const core::String_t &name, const glm::vec3 &value);
  void setUniform(const core::String_t &name, const glm::vec4 &value);
  void setUniform(const core::String_t &name, const glm::mat2 &value,
                  const core::Boolean_t &transpose = false);
  void setUniform(const core::String_t &name, const glm::mat2x3 &value,
                  const core::Boolean_t &transpose = false);
  void setUniform(const core::String_t &name, const glm::mat3x2 &value,
                  const core::Boolean_t &transpose = false);
  void setUniform(const core::String_t &name, const glm::mat3 &value,
                  const core::Boolean_t &transpose = false);
  void setUniform(const core::String_t &name, const glm::mat3x4 &value,
                  const core::Boolean_t &transpose = false);
  void setUniform(const core::String_t &name, const glm::mat4x3 &value,
                  const core::Boolean_t &transpose = false);
  void setUniform(const core::String_t &name, const glm::mat4 &value,
                  const core::Boolean_t &transpose = false);

public:
  Program();
  ~Program() override;
  void attach(const core::AutoPtr<Shader> &shader);
  core::Boolean_t link();
  const core::String_t getInfoLog() const;
  const core::Unsigned_t &getHandle() const;
  void use();
  void setUniform(const core::String_t &name, const Uniform &uniform);
};
}; // namespace firefly::gl