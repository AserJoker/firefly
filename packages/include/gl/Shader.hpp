#pragma once
#include "ShaderType.hpp"
#include "core/Object.hpp"
namespace firefly::gl {
class Shader : public core::Object {
private:
  uint32_t _handle;

public:
  Shader(SHADER_TYPE type);
  ~Shader() override;
  void setShaderSource(const std::string &source);
  bool compile();
  const std::string getInfoLog() const;
  const uint32_t &getHandle() const;
};
}; // namespace firefly::gl