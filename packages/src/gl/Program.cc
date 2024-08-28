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
void Program::setUniform(const std::string &name, const int32_t &value) {
  glUniform1i(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const uint32_t &value) {
  glUniform1ui(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const bool &value) {
  glUniform1i(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const float &value) {
  glUniform1f(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const double &value) {
  glUniform1d(getUniformLocation(name), value);
}
void Program::setUniform(const std::string &name, const glm::vec2 &value) {
  glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec3 &value) {
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec4 &value) {
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat2 &value,
                         const bool &transpose) {
  glUniformMatrix2fv(getUniformLocation(name), 1, transpose,
                     glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3 &value,
                         const bool &transpose) {
  glUniformMatrix3fv(getUniformLocation(name), 1, transpose,
                     glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat4 &value,
                         const bool &transpose) {
  glUniformMatrix4fv(getUniformLocation(name), 1, transpose,
                     glm::value_ptr(value));
}