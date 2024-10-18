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

bool Program::link() {
  glLinkProgram(_handle);
  int status;
  glGetProgramiv(_handle, GL_LINK_STATUS, &status);
  return status;
}

const std::string Program::getInfoLog() const {
  int size;
  glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &size);
  char *buf = new char[size];
  glGetProgramInfoLog(_handle, size, NULL, buf);
  std::string result = buf;
  delete[] buf;
  return result;
}

const uint32_t &Program::getHandle() const { return _handle; }

void Program::use() { glUseProgram(_handle); }

const uint32_t &Program::getUniformLocation(const std::string &name) {
  static uint32_t invalid = GL_INVALID_INDEX;
  if (!_locations.contains(name)) {
    uint32_t loc = glGetUniformLocation(_handle, name.c_str());
    if (loc == GL_INVALID_INDEX) {
      return invalid;
    }
    _locations[name] = loc;
  }
  return _locations.at(name);
}

void Program::setUniform(const std::string &name, int32_t value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(loc, value);
}
void Program::setUniform(const std::string &name, bool value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(loc, value);
}
void Program::setUniform(const std::string &name, float value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1f(loc, value);
}
void Program::setUniform(const std::string &name, const glm::vec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat2 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat4 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat2x3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2x3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3x2 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x2fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3x4 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x4fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat4x3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4x3fv(loc, 1, transpose, glm::value_ptr(value));
}