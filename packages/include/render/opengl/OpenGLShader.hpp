#pragma once
#include "core/ObjectBase.hpp"
#include <glad/glad.h>
#include <string>
#include <unordered_map>

namespace firefly::render {
class OpenGLShader : public core::ObjectBase {
public:
  enum class TYPE {
    VERTEX = GL_VERTEX_SHADER,
    TESS_CONTROL = GL_TESS_CONTROL_SHADER,
    TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
    GEOMETORY = GL_GEOMETRY_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    COMPUTE = GL_COMPUTE_SHADER
  };

private:
  uint32_t _handle;

public:
  OpenGLShader(const std::unordered_map<TYPE, std::string> &source);

  ~OpenGLShader() override;

  void use();
};
}; // namespace firefly::render