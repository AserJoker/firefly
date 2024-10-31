#pragma once
#include "ShaderType.hpp"
#include "core/Object.hpp"
namespace firefly::gl {
class Shader : public core::Object {
private:
  core::Unsigned_t _handle;

public:
  Shader(SHADER_TYPE type);
  ~Shader() override;
  void setShaderSource(const core::String_t &source);
  core::Boolean_t compile();
  const core::String_t getInfoLog() const;
  const core::Unsigned_t &getHandle() const;
};
}; // namespace firefly::gl