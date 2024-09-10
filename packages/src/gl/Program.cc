#include "gl/Program.hpp"
#include "gl/ShaderType.hpp"
#include <glm/gtc/type_ptr.hpp>
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
  if (!_locations.contains(name)) {
    auto loc = glGetUniformLocation(_handle, name.c_str());
    _locations[name] = loc;
  }
  return _locations.at(name);
}
void Program::bindUniformBlock(const std::string &name, const uint32_t &index) {
  if (!_uniformBlockLocations.contains(name)) {
    _uniformBlockLocations[name] =
        glGetUniformBlockIndex(_handle, name.c_str());
  }
  auto &loc = _uniformBlockLocations.at(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformBlockBinding(_handle, loc, index);
}
void Program::setUniform(const std::string &name, const int32_t &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(loc, value);
}
void Program::setUniform(const std::string &name, const uint32_t &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1ui(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const bool &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const float &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1f(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const double &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1d(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const glm::vec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat2 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2fv(getUniformLocation(name), 1, transpose,
                     glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3fv(getUniformLocation(name), 1, transpose,
                     glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat4 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4fv(getUniformLocation(name), 1, transpose,
                     glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::dvec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2dv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::dvec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3dv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::dvec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4dv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::ivec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2iv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::ivec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3iv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::ivec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4iv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::uvec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::uvec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::uvec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat2x3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2x3fv(getUniformLocation(name), 1, transpose,
                       glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3x2 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x2fv(getUniformLocation(name), 1, transpose,
                       glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3x4 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x4fv(getUniformLocation(name), 1, transpose,
                       glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat4x3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4x3fv(loc, 1, transpose, glm::value_ptr(value));
}