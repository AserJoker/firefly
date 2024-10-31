#include "gl/Program.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <runtime/Media.hpp>

using namespace firefly;
using namespace firefly::gl;

Program::Program() { _handle = glCreateProgram(); }

Program::~Program() { glDeleteProgram(_handle); }

void Program::attach(const core::AutoPtr<Shader> &shader) {
  glAttachShader(_handle, shader->getHandle());
}

core::Boolean_t Program::link() {
  glLinkProgram(_handle);
  int status;
  glGetProgramiv(_handle, GL_LINK_STATUS, &status);
  return status;
}

const core::String_t Program::getInfoLog() const {
  int size;
  glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &size);
  char *buf = new char[size];
  glGetProgramInfoLog(_handle, size, NULL, buf);
  core::String_t result = buf;
  delete[] buf;
  return result;
}

const core::Unsigned_t &Program::getHandle() const { return _handle; }

void Program::use() { glUseProgram(_handle); }

const core::Unsigned_t &
Program::getUniformLocation(const core::String_t &name) {
  static core::Unsigned_t invalid = GL_INVALID_INDEX;
  if (!_locations.contains(name)) {
    core::Unsigned_t loc = glGetUniformLocation(_handle, name.c_str());
    if (loc == GL_INVALID_INDEX) {
      return invalid;
    }
    _locations[name] = loc;
  }
  return _locations.at(name);
}

void Program::setUniform(const core::String_t &name, core::Integer_t value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(loc, value);
}
void Program::setUniform(const core::String_t &name, core::Boolean_t value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(loc, value);
}
void Program::setUniform(const core::String_t &name, core::Float_t value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1f(loc, value);
}
void Program::setUniform(const core::String_t &name, const glm::vec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::vec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::vec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::mat2 &value,
                         const core::Boolean_t &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::mat3 &value,
                         const core::Boolean_t &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::mat4 &value,
                         const core::Boolean_t &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::mat2x3 &value,
                         const core::Boolean_t &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2x3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::mat3x2 &value,
                         const core::Boolean_t &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x2fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::mat3x4 &value,
                         const core::Boolean_t &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x4fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const glm::mat4x3 &value,
                         const core::Boolean_t &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4x3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const core::String_t &name, const Uniform &uniform) {
  if (_uniforms.contains(name)) {
    if (_uniforms.at(name) == uniform) {
      return;
    }
  }
  _uniforms[name] = uniform;
  switch (uniform.getType()) {
  case UniformType::BOOLEAN:
    return setUniform(name, uniform.get<Boolean_t>());
  case UniformType::INTEGER:
    return setUniform(name, uniform.get<Intgeter_t>());
  case UniformType::FLOAT:
    return setUniform(name, uniform.get<Float_t>());
  case UniformType::VEC2:
    return setUniform(name, uniform.get<Vector2_t>());
  case UniformType::VEC3:
    return setUniform(name, uniform.get<Vector3_t>());
  case UniformType::VEC4:
    return setUniform(name, uniform.get<Vector4_t>());
  case UniformType::MAT2:
    return setUniform(name, uniform.get<Matrix2x2_t>());
  case UniformType::MAT3:
    return setUniform(name, uniform.get<Matrix3x3_t>());
  case UniformType::MAT4:
    return setUniform(name, uniform.get<Matrix4x4_t>());
  case UniformType::MAT2x3:
    return setUniform(name, uniform.get<Matrix2x3_t>());
  case UniformType::MAT3x2:
    return setUniform(name, uniform.get<Matrix3x2_t>());
  case UniformType::MAT3x4:
    return setUniform(name, uniform.get<Matrix3x4_t>());
  case UniformType::MAT4x3:
    return setUniform(name, uniform.get<Matrix4x3_t>());
  }
}