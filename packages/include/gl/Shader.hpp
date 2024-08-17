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
  void setSource(const std::string &source);
  void compile();
  std::string getSource();
  std::string getInfoLog();
  SHADER_TYPE getType();
  bool isDeleted();
  bool isCompileSuccess();
  uint32_t getHandle();
};
}; // namespace firefly::gl